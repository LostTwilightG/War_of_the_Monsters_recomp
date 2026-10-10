// Camada nativa do WotM: le a cena direto da RAM do EE e desenha com raylib (sem VU1/GS).
//
// Usada de dois jeitos: pelo runtime recompilado (wotm_scene.inc, a cada quadro, sobre a RDRAM viva)
// e pelo visualizador standalone (native/viewer.cpp, sobre um dump `ram_<quadro>.bin`).
// Formatos: docs/FORMATOS.md. Este arquivo nao contem dados do jogo.
//
// Convencoes do jogo (PS2): vetores-linha (p' = p * M), mundo com Z para cima, float32 little-endian.
#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <vector>

#include "raylib.h"
#include "rlgl.h"
#include "wotm_gs.hpp"

namespace wotm {

// ---------------------------------------------------------------- RAM

struct Ram {
    const uint8_t *p = nullptr;
    size_t size = 0;

    bool ok(uint32_t a, uint32_t n = 4) const { return p && a >= 0x1000 && uint64_t(a) + n <= size; }
    uint32_t u32(uint32_t a) const { uint32_t v = 0; if (ok(a)) std::memcpy(&v, p + a, 4); return v; }
    uint16_t u16(uint32_t a) const { uint16_t v = 0; if (ok(a, 2)) std::memcpy(&v, p + a, 2); return v; }
    int16_t s16(uint32_t a) const { return int16_t(u16(a)); }
    uint8_t u8(uint32_t a) const { return ok(a, 1) ? p[a] : 0; }
    int8_t s8(uint32_t a) const { return int8_t(u8(a)); }
    float f32(uint32_t a) const { float v = 0; if (ok(a)) std::memcpy(&v, p + a, 4); return v; }
};

// Enderecos fixos do retail NTSC-U (SCUS_971.97): symbol_addrs.txt
namespace addr {
constexpr uint32_t world = 0x006F87C4;            // _worldctx *world (ep@0, skyCs@4, eo@0x20, weMat@0x30)
constexpr uint32_t worldCtx = 0x00445CD0;         // _worldctx worldCtx[5] (0x1F0 cada)
constexpr uint32_t viewInfo = 0x006E1900;         // 5 x 0x14: [0] = _cs* da camera, [1] = viewport*
constexpr uint32_t worldToScreenMat = 0x006E1FD0; // 5 x 0x40
constexpr uint32_t csActiveList = 0x0043A2C0;     // CsPool::m_activeList (CsNode: cs@0, next@4)
constexpr uint32_t csHPActiveList = 0x0043A2E0;   // CsPool::m_HPActiveList
constexpr uint32_t tempVramTexAddr = 0x006F8818;  // fim dos recursos (.RTX) na VRAM, em palavras; >> 6 = bloco base das paletas
constexpr uint32_t texInfo = 0x0050F100;         // texInfo[1000] x 16: [0] = pacote de upload da textura (RAM), [4] = residencia/VRAM
}

// ---------------------------------------------------------------- matrizes (vetores-linha)

struct M4 {
    float m[4][4];
};

inline M4 ident() {
    M4 r{};
    for (int i = 0; i < 4; ++i) r.m[i][i] = 1.f;
    return r;
}

// a primeiro, depois b
inline M4 mul(const M4 &a, const M4 &b) {
    M4 r{};
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) {
            float s = 0;
            for (int k = 0; k < 4; ++k) s += a.m[i][k] * b.m[k][j];
            r.m[i][j] = s;
        }
    return r;
}

inline M4 translateM(float x, float y, float z) {
    M4 r = ident();
    r.m[3][0] = x; r.m[3][1] = y; r.m[3][2] = z;
    return r;
}

inline M4 scaleM(float x, float y, float z) {
    M4 r = ident();
    r.m[0][0] = x; r.m[1][1] = y; r.m[2][2] = z;
    return r;
}

inline M4 readM4(const Ram &ram, uint32_t a) {
    M4 r{};
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) r.m[i][j] = ram.f32(a + 16 * i + 4 * j);
    return r;
}

inline void xform(const M4 &m, float x, float y, float z, float out[3]) {
    out[0] = x * m.m[0][0] + y * m.m[1][0] + z * m.m[2][0] + m.m[3][0];
    out[1] = x * m.m[0][1] + y * m.m[1][1] + z * m.m[2][1] + m.m[3][1];
    out[2] = x * m.m[0][2] + y * m.m[1][2] + z * m.m[2][2] + m.m[3][2];
}

// ---------------------------------------------------------------- malha (polyPkt)

// Um HierObject::polyPkt e uma sub-cadeia de DMA (tag QWC/ID=ret + 8 bytes) com um fluxo VIF para o microcodigo do VU1:
//   UNPACK V3-32 @0xB5   posicoes (ate 190)        UNPACK V3-8 @0x175   normais (signed /127)
//   por secao de strip (terminada por MSCNT): tag GIF em H (V4-32, NLOOP = n vertices), depois 3 qwords por vertice
//   (ST, RGBAQ, XYZF): S-8 @H+1 indice da posicao, V4-8 @H+2 RGBA (0x80 = 1.0), V2-16 @H+3 UV (4096 = 1.0).
//   STMASK 0xBFBFBFBF + S-8 @H+3+3k marca ADC (nao desenha) no vertice k: reinicia o strip.
struct Vtx {
    float x, y, z;
    float u, v;
    uint8_t r, g, b, a;
};

struct MeshData {
    std::vector<Vtx> tris;   // lista de triangulos (3 vertices por triangulo)
    uint32_t qwc = 0;
    uint32_t sig = 0;        // assinatura barata para detectar RAM reaproveitada por outro nivel
    uint16_t texId = 0;
    bool valid = false;
    bool abe = false;        // PRIM.ABE da tag GIF: so entao o alfa de vertice vale (blend); senao opaco
};

inline MeshData decodeObject(const Ram &ram, uint32_t node) {
    MeshData md;
    const uint32_t pp = ram.u32(node + 4) & 0x0FFFFFFF;
    if (!ram.ok(pp, 16)) return md;
    const uint32_t tag = ram.u32(pp);
    const uint32_t qwc = tag & 0xFFFF;
    const uint32_t nwords = (qwc + 1) * 4 - 2;
    const uint32_t base = pp + 8;
    if (qwc == 0 || !ram.ok(base, nwords * 4)) return md;
    md.qwc = qwc;
    md.sig = tag ^ ram.u32(pp + 8) ^ (ram.u32(pp + 12) * 31u) ^ (ram.u32(pp + 4 * (nwords + 1)) * 17u);
    md.texId = ram.u16(node + 0x50);
    md.valid = true;

    struct P3 { float x, y, z; };
    std::vector<P3> pos;
    uint32_t hdrAddr = 0, nloop = 0;
    bool haveHdr = false;
    std::vector<uint8_t> idx;
    std::vector<std::array<uint8_t, 4>> rgba;
    std::vector<std::array<int16_t, 2>> uv;
    std::vector<uint32_t> adc;
    uint32_t mask = 0;

    auto flush = [&]() {
        if (haveHdr && !idx.empty()) {
            const uint32_t n = std::min<uint32_t>(nloop, uint32_t(idx.size()));
            std::vector<bool> restart(n + 1, false);
            for (uint32_t k : adc) if (k < n) restart[k] = true;
            std::vector<Vtx> strip;
            auto emit = [&]() {
                for (size_t k = 2; k < strip.size(); ++k) {
                    const Vtx &a = strip[(k & 1) ? k - 1 : k - 2];
                    const Vtx &b = strip[(k & 1) ? k - 2 : k - 1];
                    const Vtx &c = strip[k];
                    // descarta triangulos degenerados (juncao de strips por vertice repetido)
                    auto same = [](const Vtx &p, const Vtx &q) { return p.x == q.x && p.y == q.y && p.z == q.z; };
                    if (same(a, b) || same(b, c) || same(a, c)) continue;
                    md.tris.push_back(a); md.tris.push_back(b); md.tris.push_back(c);
                }
                strip.clear();
            };
            for (uint32_t k = 0; k < n; ++k) {
                if (restart[k]) emit();
                const uint32_t j = idx[k];
                const P3 p = j < pos.size() ? pos[j] : P3{0, 0, 0};
                Vtx v{};
                v.x = p.x; v.y = p.y; v.z = p.z;
                if (k < rgba.size()) { v.r = rgba[k][0]; v.g = rgba[k][1]; v.b = rgba[k][2]; v.a = rgba[k][3]; }
                else { v.r = v.g = v.b = v.a = 128; }
                if (k < uv.size()) { v.u = uv[k][0] / 4096.f; v.v = uv[k][1] / 4096.f; }
                strip.push_back(v);
            }
            emit();
        }
        haveHdr = false; idx.clear(); rgba.clear(); uv.clear(); adc.clear();
    };

    uint32_t i = 0;
    while (i < nwords) {
        const uint32_t w = ram.u32(base + 4 * i);
        const uint32_t cmd = (w >> 24) & 0x7F, num = (w >> 16) & 0xFF, imm = w & 0xFFFF;
        const uint32_t a = base + 4 * (i + 1);
        if (cmd >= 0x60) {
            const uint32_t vn = (cmd >> 2) & 3, vl = cmd & 3, n = num ? num : 256, addr = imm & 0x3FF;
            static const uint32_t comps[4] = {1, 2, 3, 4}, bits[4] = {32, 16, 8, 5};
            const uint32_t nw = vl == 3 ? (n * 16 + 31) / 32 : (n * bits[vl] * comps[vn] + 31) / 32;
            if (vn == 2 && vl == 0 && addr == 0xB5) {                        // V3-32 posicoes
                if (haveHdr) flush();
                pos.resize(n);
                for (uint32_t k = 0; k < n; ++k) pos[k] = {ram.f32(a + 12 * k), ram.f32(a + 12 * k + 4), ram.f32(a + 12 * k + 8)};
            } else if (vn == 3 && vl == 0 && n == 1) {                        // V4-32: tag GIF
                hdrAddr = addr; nloop = ram.u32(a) & 0x7FFF; haveHdr = true;
                if (((ram.u32(a + 4) >> 15) >> 6) & 1) md.abe = true;
            } else if (haveHdr && vn == 0 && vl == 2 && (mask & 0xFF) == 0xBF) {   // S-8 sob mascara: ADC
                for (uint32_t k = 0; k < n; ++k) {
                    const int e = int(addr) - int(hdrAddr) - 3 + 3 * int(k);
                    if (e >= 0 && e % 3 == 0) adc.push_back(uint32_t(e / 3));
                }
            } else if (haveHdr && vn == 0 && vl == 2 && addr == hdrAddr + 1) {      // S-8 indices
                idx.resize(n);
                for (uint32_t k = 0; k < n; ++k) idx[k] = ram.u8(a + k);
            } else if (haveHdr && vn == 3 && vl == 2 && addr == hdrAddr + 2) {      // V4-8 RGBA
                rgba.resize(n);
                for (uint32_t k = 0; k < n; ++k) rgba[k] = {ram.u8(a + 4 * k), ram.u8(a + 4 * k + 1), ram.u8(a + 4 * k + 2), ram.u8(a + 4 * k + 3)};
            } else if (haveHdr && vn == 1 && vl == 1 && addr == hdrAddr + 3) {      // V2-16 UV
                uv.resize(n);
                for (uint32_t k = 0; k < n; ++k) uv[k] = {ram.s16(a + 4 * k), ram.s16(a + 4 * k + 2)};
            }
            i += 1 + nw;
            continue;
        }
        uint32_t extra = 0;
        if (cmd == 0x20) { mask = ram.u32(a); extra = 1; }
        else if (cmd == 0x30 || cmd == 0x31) extra = 4;
        else if (cmd == 0x4A) extra = (num ? num : 256) * 2;
        else if (cmd == 0x50 || cmd == 0x51) extra = (imm ? imm : 65536) * 4;
        else if (cmd == 0x14 || cmd == 0x15 || cmd == 0x17) flush();   // MSCAL/MSCALF/MSCNT: fim da secao
        i += 1 + extra;
    }
    flush();
    return md;
}

// ---------------------------------------------------------------- cena

struct DrawItem {
    M4 m;
    uint32_t node;   // HierObject
};

struct Camera {
    float pos[3] = {0, 0, 0};
    float yaw = 0, pitch = 0;   // radianos; yaw 0 olha para +X, Z para cima
};

// Camera do jogo para uma vista: o `_cs` de viewInfo[view] (posicao em +0x10, matriz em +0x20: as linhas sao os eixos do
// olho em coordenadas de mundo = direita, FRENTE, cima). worldToScreenMat tem foco 384 sobre 640 px (HFOV ~79,6 graus) e,
// em 4:3, o VFOV e ~63,7 graus (o eixo de profundidade do olho e o Y).
struct GameCamera {
    bool ok = false;
    float pos[3] = {0, 0, 0};
    float fwd[3] = {0, 1, 0};
    float up[3] = {0, 0, 1};
    float fovy = 63.7f;   // graus
};

inline GameCamera readGameCamera(const Ram &ram, int view = 0) {
    GameCamera c;
    const uint32_t cs = ram.u32(addr::viewInfo + 0x14 * view);
    if (!ram.ok(cs, 0x60)) return c;
    for (int i = 0; i < 3; ++i) {
        c.pos[i] = ram.f32(cs + 0x10 + 4 * i);
        c.fwd[i] = ram.f32(cs + 0x20 + 16 * 1 + 4 * i);
        c.up[i] = ram.f32(cs + 0x20 + 16 * 2 + 4 * i);
    }
    const float f0 = ram.f32(addr::worldToScreenMat + 0x40 * view);          // 384: foco horizontal
    const float fv = -ram.f32(addr::worldToScreenMat + 0x40 * view + 0x18);  // 180,48: foco vertical por campo (entrelacado)
    if (f0 > 1.f && fv > 1.f) c.fovy = 2.f * std::atan(224.f / (2.f * fv)) * 180.f / 3.14159265f;
    c.ok = std::isfinite(c.pos[0]) && std::isfinite(c.fwd[0]) && (std::fabs(c.fwd[0]) + std::fabs(c.fwd[1]) + std::fabs(c.fwd[2])) > 0.1f;
    return c;
}

class Scene {
public:
    std::vector<DrawItem> items;
    std::vector<DrawItem> sky;   // ceu / nuvens (desenhados antes, centrados na camera)
    uint32_t visited = 0;
    uint32_t byOp[64] = {};

    gs::Vram vram;   // so para depuracao: a VRAM do dump guarda paletas velhas (restos do menu)

    // Paletas da fase, lidas do `.RTX` do disco: entradas de `(palavra0 >> 2) * 16` bytes com cabecalho de 16 bytes (descritor GS
    // em +8: DBP relativo no meia-palavra +0xA, TW/TH nos bits 40..47) e o corpo em ordem logica de paleta (entrada i = palavra i):
    // 16x16 CT32 = 256 cores, 8x2 CT32 = 16 cores. O TEX0.CBP do objeto e `base + DBP`, com base = tempVramTexAddr >> 6.
    std::unordered_map<uint32_t, std::vector<uint32_t>> rtxPal;

    bool loadRtx(const char *path) {
        rtxPal.clear();
        std::vector<uint8_t> raw;
        if (FILE *f = std::fopen(path, "rb")) {
            std::fseek(f, 0, SEEK_END);
            raw.resize(size_t(std::ftell(f)));
            std::fseek(f, 0, SEEK_SET);
            if (std::fread(raw.data(), 1, raw.size(), f) != raw.size()) raw.clear();
            std::fclose(f);
        }
        if (raw.size() < 34) return false;
        std::vector<uint8_t> data;
        static const uint8_t kMagic[4] = {'I', 'E', 3, 4};
        if (std::memcmp(raw.data(), kMagic, 4) == 0) {   // zip com o magic trocado, um arquivo em deflate cru
            uint32_t csize = 0;
            uint16_t nlen = 0, xlen = 0;
            std::memcpy(&csize, raw.data() + 18, 4);
            std::memcpy(&nlen, raw.data() + 26, 2);
            std::memcpy(&xlen, raw.data() + 28, 2);
            const size_t off = 30 + nlen + xlen;
            if (off + csize > raw.size()) return false;
            int outSize = 0;
            unsigned char *d = DecompressData(raw.data() + off, int(csize), &outSize);
            if (!d || outSize <= 0) return false;
            data.assign(d, d + outSize);
            MemFree(d);
        } else {
            data = raw;
        }
        size_t off = 0;
        while (off + 16 <= data.size()) {
            uint32_t w[4];
            std::memcpy(w, data.data() + off, 16);
            if (w[0] == 0) break;
            const size_t sz = size_t(w[0] >> 2) * 16;
            if (sz < 16 || off + sz > data.size()) break;
            const uint64_t desc = uint64_t(w[2]) | (uint64_t(w[3]) << 32);
            const uint32_t psm = uint32_t(desc >> 32) & 0x3F, tw = uint32_t(desc >> 40) & 15, th = uint32_t(desc >> 44) & 15;
            if (psm == 0 && ((tw == 4 && th == 4) || (tw == 3 && th == 1))) {
                std::vector<uint32_t> p((sz - 16) / 4);
                std::memcpy(p.data(), data.data() + off + 16, p.size() * 4);
                rtxPal[(w[2] >> 16) & 0xFFFF] = std::move(p);
            }
            off += sz;
        }
        return !rtxPal.empty();
    }

    void invalidate() {
        cache_.clear();
        for (auto &kv : tex_) if (kv.second.id) UnloadTexture(kv.second);
        tex_.clear();
    }

    // TEX0 do objeto (gsTexCtx[0]); 0 quando o objeto nao tem textura
    static uint64_t objectTex0(const Ram &ram, uint32_t node) {
        if (ram.u16(node + 0x5A) == 0) return 0;
        return uint64_t(ram.u32(node + 0x70)) | (uint64_t(ram.u32(node + 0x74)) << 32);
    }

    // Chave de textura de um objeto: id no `texInfo` + paleta (CBP/CSA/CPSM do TEX0). 0 = sem textura.
    static uint64_t texKey(const Ram &ram, uint32_t node) {
        const uint64_t t0 = objectTex0(ram, node);
        const uint32_t tid = ram.u16(node + 0x50);
        if (!t0 || !tid) return 0;
        const gs::Tex0 t = gs::Tex0::decode(t0);
        return (uint64_t(tid) << 40) | (uint64_t(t.cbp) << 12) | (uint64_t(t.csa) << 4) | t.cpsm | (1ull << 62);
    }

    // Textura do objeto: os pixels vem do pacote de upload apontado por `texInfo[texId]` (pixels lineares a partir de +0x80,
    // descritor em +0x24 largura, +0x26 altura, +0x2B PSM); a paleta vem da VRAM (carregada junto com a fase).
    bool decodeTex(const Ram &ram, uint32_t node, std::vector<uint32_t> &px, uint32_t &w, uint32_t &h) const {
        const uint64_t t0v = objectTex0(ram, node);
        if (!t0v) return false;
        const gs::Tex0 t0 = gs::Tex0::decode(t0v);
        const uint32_t ptr = ram.u32(addr::texInfo + 16 * ram.u16(node + 0x50)) & 0x0FFFFFFF;
        if (!ram.ok(ptr + 0x80)) return false;
        uint32_t pal[256] = {};
        const uint32_t psm = ram.u8(ptr + 0x2B);
        if (psm == gs::T8 || psm == gs::T4) {
            // A paleta vem do .RTX carregado (loadRtx); sem ela a textura cai para a cor de vertice.
            const uint32_t base = ram.u32(addr::tempVramTexAddr) >> 6;
            if (t0.cbp < base) return false;
            const auto it = rtxPal.find(t0.cbp - base);
            if (it == rtxPal.end()) return false;
            for (size_t i = 0; i < it->second.size() && i < 256; ++i) pal[i] = gs::fixAlpha(it->second[i]);
        }
        w = ram.u16(ptr + 0x24); h = ram.u16(ptr + 0x26);
        return gs::decodeUpload(ram.p + ptr + 0x80, ram.size - ptr - 0x80, w, h, psm, t0.csa, pal, px);
    }

    unsigned textureId(const Ram &ram, uint32_t node) {
        const uint64_t key = texKey(ram, node);
        if (!key) return 0;
        auto it = tex_.find(key);
        if (it != tex_.end()) return it->second.id;
        Texture2D t{};
        std::vector<uint32_t> px;
        uint32_t w = 0, h = 0;
        if (decodeTex(ram, node, px, w, h)) {
            Image img{};
            img.data = px.data(); img.width = int(w); img.height = int(h); img.mipmaps = 1;
            img.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
            t = LoadTextureFromImage(img);
            if (t.id) { SetTextureFilter(t, TEXTURE_FILTER_BILINEAR); SetTextureWrap(t, TEXTURE_WRAP_REPEAT); }
        }
        tex_[key] = t;
        return t.id;
    }

    // Reune os objetos visiveis a partir de world->ep e das listas de CS.
    void collect(const Ram &ram, int maxLod = 0) {
        items.clear();
        sky.clear();
        visited = 0;
        std::memset(byOp, 0, sizeof byOp);
        seen_.clear();
        lod_ = maxLod;
        const uint32_t wc = ram.u32(addr::world);
        if (ram.ok(wc, 0x40)) {
            const uint32_t ep = ram.u32(wc);
            if (ram.ok(ep)) walk(ram, ep, ident(), 0);
        }
        // ceu da vista 0: skyCs/skyCs2/skyClouds de worldCtx[0] (+4/+8/+0xC); hierTraceSky so olha o epNode e desenha no olho
        {
            std::vector<DrawItem> world;
            world.swap(items);
            const GameCamera gc = readGameCamera(ram, 0);
            for (uint32_t off : {4u, 8u, 0xCu}) drawCs(ram, ram.u32(addr::worldCtx + off), false, gc.ok ? gc.pos : nullptr);
            sky.swap(items);
            items.swap(world);
        }
        csList(ram, addr::csActiveList);
        csList(ram, addr::csHPActiveList);
    }

    const MeshData *mesh(const Ram &ram, uint32_t node) {
        const uint32_t pp = ram.u32(node + 4) & 0x0FFFFFFF;
        auto it = cache_.find(pp);
        if (it != cache_.end()) {
            // revalida a assinatura (a RAM do nivel pode ter sido reaproveitada)
            const uint32_t tag = ram.u32(pp);
            const uint32_t qwc = tag & 0xFFFF, nwords = (qwc + 1) * 4 - 2;
            const uint32_t sig = tag ^ ram.u32(pp + 8) ^ (ram.u32(pp + 12) * 31u) ^ (ram.u32(pp + 4 * (nwords + 1)) * 17u);
            if (it->second->sig == sig) return it->second.get();
        }
        auto md = std::make_shared<MeshData>(decodeObject(ram, node));
        cache_[pp] = md;
        return md.get();
    }

    // Desenha com o raylib (deve estar dentro de BeginMode3D).
    void draw(const Ram &ram, bool wire = false) {
        rlDisableDepthTest();   // o ceu e desenhado primeiro, centrado no olho, sem profundidade
        drawList(ram, sky, wire);
        rlDrawRenderBatchActive();
        rlEnableDepthTest();
        drawList(ram, items, wire);
    }

    void drawList(const Ram &ram, const std::vector<DrawItem> &list, bool wire) {
        rlDisableBackfaceCulling();
        std::vector<std::pair<uint64_t, uint32_t>> order;   // (tex0, indice do item)
        order.reserve(list.size());
        for (uint32_t i = 0; i < list.size(); ++i) order.push_back({wire ? 0 : texKey(ram, list[i].node), i});
        std::stable_sort(order.begin(), order.end(), [](const auto &a, const auto &b) { return a.first < b.first; });

        const unsigned white = rlGetTextureIdDefault();
        uint64_t cur = ~0ull;
        bool open = false;
        for (const auto &o : order) {
            const DrawItem &it = list[o.second];
            const MeshData *md = mesh(ram, it.node);
            if (!md || !md->valid || md->tris.empty()) continue;
            if (o.first != cur || !open) {
                if (open) { rlEnd(); rlDrawRenderBatchActive(); }   // um lote por textura (evita o alinhamento automatico do rlgl)
                cur = o.first;
                const unsigned id = wire ? 0 : textureId(ram, it.node);
                rlBegin(wire ? RL_LINES : RL_TRIANGLES);   // rlBegin com outro modo reseta a textura do lote: setar depois
                rlSetTexture(id ? id : white);
                open = true;
            }
            const unsigned tid = (wire || !cur) ? 0 : textureId(ram, it.node);
            const auto &t = md->tris;
            for (size_t k = 0; k + 2 < t.size(); k += 3) {
                float p[3][3];
                for (int c = 0; c < 3; ++c) xform(it.m, t[k + c].x, t[k + c].y, t[k + c].z, p[c]);
                for (int c = 0; c < 3; ++c) {
                    const int cs[2] = {c, (c + 1) % 3};
                    for (int e = 0; e < (wire ? 2 : 1); ++e) {
                        const Vtx &v = t[k + cs[e]];
                        rlColor4ub(uint8_t(std::min(255, v.r * 2)), uint8_t(std::min(255, v.g * 2)), uint8_t(std::min(255, v.b * 2)), md->abe ? uint8_t(std::min(255, v.a * 2)) : uint8_t(255));
                        if (tid) rlTexCoord2f(v.u, v.v);
                        rlVertex3f(p[cs[e]][0], p[cs[e]][1], p[cs[e]][2]);
                    }
                }
            }
        }
        if (open) rlEnd();
        rlSetTexture(0);
        rlEnableBackfaceCulling();
    }

private:
    std::unordered_map<uint32_t, std::shared_ptr<MeshData>> cache_;
    std::unordered_map<uint64_t, Texture2D> tex_;
    std::vector<uint32_t> seen_;   // nos ja visitados neste quadro (ciclos / DAG)
    int lod_ = 0;

    bool already(uint32_t n) {
        // lista pequena e ordenada por insercao; o jogo tem ate alguns milhares de nos
        for (uint32_t s : seen_) if (s == n) return true;
        seen_.push_back(n);
        return false;
    }

    void kids(const Ram &ram, uint32_t arr, uint32_t n, const M4 &m, int depth) {
        if (n > 4096) n = 4096;
        for (uint32_t i = 0; i < n; ++i) {
            const uint32_t c = ram.u32(arr + 4 * i);
            if (ram.ok(c)) walk(ram, c, m, depth + 1);
        }
    }

    void walk(const Ram &ram, uint32_t node, const M4 &m, int depth) {
        if (depth > 64 || visited > 200000 || !ram.ok(node)) return;
        // O mesmo no pode aparecer com matrizes diferentes (instancias): so corta ciclos profundos
        if (depth > 24 && already(node)) return;
        ++visited;
        const uint32_t op = ram.u32(node) & 0x3F;
        ++byOp[op];
        switch (op) {
        case 0: items.push_back({m, node}); break;                                           // OBJECT
        case 1: kids(ram, node + 0x20, ram.u16(node + 8), m, depth); break;                  // GROUP
        case 2: {                                                                           // LOD
            const uint32_t n = ram.u32(node + 4);
            if (n > 0 && n < 16) {
                const uint32_t i = std::min<uint32_t>(lod_, n - 1);
                const uint32_t c = ram.u32(node + 0x20 + 16 * i + 8);
                if (ram.ok(c)) walk(ram, c, m, depth + 1);
            }
            break;
        }
        case 3:                                                                             // TRANSLATE
            kids(ram, node + 0x1C, ram.u32(node + 8),
                 mul(translateM(ram.f32(node + 0x10), ram.f32(node + 0x14), ram.f32(node + 0x18)), m), depth);
            break;
        case 4: kids(ram, node + 0x50, ram.u32(node + 8), mul(readM4(ram, node + 0x10), m), depth); break;   // ROTATE
        case 30:                                                                            // SCALE
            kids(ram, node + 0x2C, ram.u32(node + 8),
                 mul(mul(scaleM(ram.f32(node + 0x20), ram.f32(node + 0x24), ram.f32(node + 0x28)),
                         translateM(ram.f32(node + 0x10), ram.f32(node + 0x14), ram.f32(node + 0x18))), m), depth);
            break;
        case 6: {                                                                           // SWITCH
            const uint32_t which = ram.u8(node + 8), n = ram.u8(node + 0xB);
            if (which < n) { const uint32_t c = ram.u32(node + 0xC + 4 * which); if (ram.ok(c)) walk(ram, c, m, depth + 1); }
            break;
        }
        case 8: { const uint32_t c = ram.u32(node + 4); if (ram.ok(c)) walk(ram, c, m, depth + 1); break; }   // CONTROL: child1
        case 23: { const uint32_t c = ram.u32(node + 4); if (ram.ok(c)) walk(ram, c, m, depth + 1); break; } // ACTION_DATA
        case 25: kids(ram, node + 0x2C, ram.u32(node + 0x28), m, depth); break;             // CHAR_INSTANCE
        case 39: { const uint32_t c = ram.u32(node + 0xC); if (ram.ok(c)) walk(ram, c, m, depth + 1); break; } // INTERACTIVE
        case 31: {                                                                          // DESTRUCTIBLE: estado 0 (intacto)
            if (ram.u16(node + 0xA)) { const uint32_t c = ram.u32(node + 0xC); if (ram.ok(c)) walk(ram, c, m, depth + 1); }
            break;
        }
        default: break;   // colisao, luz, particulas, animacao...: proximos marcos
        }
    }

    void csList(const Ram &ram, uint32_t head) {
        uint32_t n = ram.u32(head + 4);
        for (int guard = 0; ram.ok(n) && n != head && guard < 4096; ++guard, n = ram.u32(n + 4)) {
            drawCs(ram, ram.u32(n), true);
        }
    }

    // Um `_cs` (instancia com matriz e posicao proprias): epNode em +0, drawMe +0xC, trans +0x10, mat +0x20, scaleMe +0xF, scale +0x80
    void drawCs(const Ram &ram, uint32_t cs, bool needDrawMe, const float *at = nullptr) {
        if (!ram.ok(cs, 0xB0) || (needDrawMe && !ram.u8(cs + 0xC))) return;
        const uint32_t ep = ram.u32(cs);
        if (!ram.ok(ep)) return;
        M4 m = readM4(ram, cs + 0x20);
        m.m[3][0] = ram.f32(cs + 0x10); m.m[3][1] = ram.f32(cs + 0x14); m.m[3][2] = ram.f32(cs + 0x18); m.m[3][3] = 1.f;
        m.m[0][3] = m.m[1][3] = m.m[2][3] = 0.f;
        if (at) { m.m[3][0] = at[0]; m.m[3][1] = at[1]; m.m[3][2] = at[2]; }
        if (ram.u8(cs + 0xF)) {   // scaleMe
            const float sx = ram.f32(cs + 0x80), sy = ram.f32(cs + 0x84), sz = ram.f32(cs + 0x88);
            m = mul(scaleM(sx, sy, sz), m);
        }
        walk(ram, ep, m, 0);
    }
};

}  // namespace wotm
