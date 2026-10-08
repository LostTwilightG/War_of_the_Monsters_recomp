#include "common.h"

struct Monster;
struct _ParticleType;
enum FxTextureId { FX_TEXTURE_14 = 0x14 };

_ParticleType *chainCreate(float a, float b, int n, unsigned flags, FxTextureId tex, float c);

class MonkeyChains {
public:
    class Chain {
    public:
        char data[0x60];

        void init(Monster *m, _ParticleType *t, int id);
        void update(Monster *m);
    };

    Monster *owner;
    _ParticleType *type;
    char pad8[8];
    Chain chains[6];

    void init(Monster *m);
    int update(void);
};

#ifdef NON_MATCHING
/* 31/61 words, untuned: chain pointer temporaries */
void MonkeyChains::init(Monster *m)
{
    FxTextureId tex = FX_TEXTURE_14;
    Chain *c = &chains[1];

    owner = m;
    type = chainCreate(20.0f, 0.22f, 4, 0x8030383C, tex, 0.0f);
    chains[0].init(m, type, 0x816);
    c = &chains[1];
    c->init(m, type, 0x817);
    c = &chains[2];
    c->init(m, type, 0x818);
    c = &chains[3];
    c->init(m, type, 0x819);
    c = &chains[4];
    c->init(m, type, 0x81A);
    c = &chains[5];
    c->init(m, type, 0x81B);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/MonkeyChains", init__12MonkeyChainsP7Monster);
#endif
int MonkeyChains::update(void)
{
    Chain *c = chains;
    int i;

    for (i = 6; i != 0; i--, c++)
        c->update(owner);
    return 1;
}
INCLUDE_ASM("asm/nonmatchings/game/MonkeyChains", init__Q212MonkeyChains5ChainP7MonsterP13_ParticleTypei);
INCLUDE_ASM("asm/nonmatchings/game/MonkeyChains", update__Q212MonkeyChains5ChainP7Monster);
