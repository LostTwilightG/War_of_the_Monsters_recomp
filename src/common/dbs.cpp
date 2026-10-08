#include "common.h"
#include "memory_stack.h"

extern "C" int printf(const char *, ...);
extern "C" int sprintf(char *, const char *, ...);
extern "C" int strcmp(const char *, const char *);
extern "C" void *memset(void *, int, unsigned);

enum _vramAddrs { VRAM_ADDRS_DUMMY };

int fileReadf(char *name, void *dest);
char *getNextTexLoadAddr(void);
char *getNgpAddr(int i);
char *getGenericName(int i);
int getMaxTexId(int i);
int getMaxTexAddr(int i);
int getMaxResAddr(int i);

extern char D_006F85F0[]; /* "shell" */
extern char D_006F85F8[]; /* "SHELL" */
extern char D_006F8600[]; /* "load" */
extern char D_006F8608[]; /* "LOAD" */
extern char D_006F8610[]; /* "ui" */
extern char D_006F8618[]; /* "UI" */
extern char D_006F8620[]; /* "shella" */
extern char D_006F8628[]; /* "SHELLA" */

INCLUDE_ASM("asm/nonmatchings/common/dbs", dbsPush__FPP9_hierheadP8_fvectorUiPA3_fP17_animCharInstance);
INCLUDE_ASM("asm/nonmatchings/common/dbs", dbsPop__FP9_dbsstackPP9_hierhead);
INCLUDE_ASM("asm/nonmatchings/common/dbs", dbsTraverse__FPP9_hierheadPFP9_hierheadP8_fvectorPA3_f_vP8_fvector);
/* Relocates the monster image `idx` (mon/<name>.ptr, loaded to a temporary buffer on the memory stack). Images are linked for address
   0xA00000. The pointer file holds four lists, each a count followed by byte offsets into the image (0 = unused):
   0. the image header: words 1..N (N = word 0) are pointers that move by (load address - 0xA00000);
   1. words that are pointers (same shift);
   2. 16-bit texture ids, shifted by the highest id of the previous image;
   3. GS words: TEX0.TBP0 moves by the previous image's texture end; words tagged 0x1B/0x2C/0x24 also move their low 14 bits by its resource end. */
#ifdef NON_MATCHING
/* 48/206 words: untuned, from the m2c draft + asm */
void dbsRelocateViaPtrListFile(int idx, _vramAddrs vram)
{
    char name[0x18];
    int *file;
    char *img;
    int base;
    int size;
    int n1;
    int n2;
    int n3;
    int k;

    if (idx == 0)
        return;
    MemoryStack::global.pushMark();
    file = (int *)(((int)MemoryStack::global.low + 0xF) & ~0xF);
    MemoryStack::global.low = (char *)file + 0xD40;
    memset(name, 0, 0x18);
    base = (int)getNgpAddr(idx) + (int)0xFF600000;
    sprintf(name, "mon/%s.ptr", getGenericName(idx));
    size = fileReadf(name, file);
    if (size < 0)
        printf("Could not read pointer file \"%s\"!!
", name);
    if (size > 0x30D40)
        printf("Pointer file too big.  Prepare for crash...
");
    img = getNgpAddr(idx);
    n1 = *(int *)img;
    for (k = 1; k <= n1; k++)
        ((int *)img)[k] += base;
    n1 = file[0];
    for (k = 0; k < n1; k++)
        *(int *)(img + file[1 + k]) += base;
    printf("Finished ptrs
");
    n2 = file[1 + n1];
    for (k = 0; k < n2; k++) {
        int off = file[2 + n1 + k];
        unsigned short *id = (unsigned short *)(img + off);

        if (off != 0 && *id != 0)
            *id += getMaxTexId(idx - 1);
    }
    printf("Finished texIds
");
    n3 = file[2 + n1 + n2];
    for (k = 0; k < n3; k++) {
        int off = file[3 + n1 + n2 + k];
        unsigned *w = (unsigned *)(img + off);

        if (off != 0) {
            unsigned lo;

            w[1] = (w[1] & ~(0x3FFFu << 5)) | ((((w[1] >> 5) & 0x3FFF) + (getMaxTexAddr(idx - 1) & 0xFFFF)) & 0x3FFF) << 5;
            lo = w[0] & 0x03F00000;
            if (lo == 0x01B00000 || lo == 0x02C00000 || lo == 0x02400000)
                w[0] = (w[0] & ~0x3FFFu) | (((w[0] & 0x3FFF) + (getMaxResAddr(idx - 1) & 0xFFFF)) & 0x3FFF);
        }
    }
    MemoryStack::global.low = (char *)MemoryStack::global.mark;
    MemoryStack::global.mark = *(int *)MemoryStack::global.mark;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/dbs", dbsRelocateViaPtrListFile__Fi10_vramAddrs);
#endif
INCLUDE_ASM("asm/nonmatchings/common/dbs", D_006F3A58);
/* Relocates the texture base pointers (TEX0.TBP0, 14 bits at bit 37) of the loaded .NGP image of file zero by `vram` / 64.
   The pointer file (lvl/<name>.PTR, shell/<x>.ptr) holds three lists, each a count followed by that many words; the first two
   are skipped here, the third lists byte offsets into the image (0 = unused). */
#ifdef NON_MATCHING
/* 2/196 words: untuned, from the m2c draft + asm (TBP0 patch done on the high word) */
void dbsRelocateFileZero(_vramAddrs vram, bool b)
{
    char name[0x18] = "";
    int *list = (int *)getNextTexLoadAddr();
    char *ngp = getNgpAddr(0);
    int *p;
    int n;

    if (strcmp(getGenericName(0), D_006F85F0) == 0 || strcmp(getGenericName(0), D_006F85F8) == 0)
        sprintf(name, "shell/shell.ptr");
    else if (strcmp(getGenericName(0), D_006F8600) == 0 || strcmp(getGenericName(0), D_006F8608) == 0)
        sprintf(name, "shell/load.ptr");
    else if (strcmp(getGenericName(0), D_006F8610) == 0 || strcmp(getGenericName(0), D_006F8618) == 0)
        sprintf(name, "shell/ui.ptr");
    else if (strcmp(getGenericName(0), D_006F8620) == 0 || strcmp(getGenericName(0), D_006F8628) == 0)
        sprintf(name, "shell/shella.ptr");
    else if (strcmp(getGenericName(0), "preshell") == 0 || strcmp(getGenericName(0), "PRESHELL") == 0)
        sprintf(name, "shell/preshell.ptr");
    else
        sprintf(name, "lvl/%s.PTR", getGenericName(0));
    if (fileReadf(name, list) < 0)
        printf("dbsRelocateFileZero::Could not read pointer file (%s)!!
", name);
    p = list + list[0] + 1;
    p = p + p[0] + 1;
    n = p[0];
    p++;
    if (n > 0) {
        int delta = (int)vram / 64;

        for (; n != 0; n--, p++) {
            if (*p != 0) {
                unsigned *hi = (unsigned *)(ngp + *p + 4); /* TBP0 is bits 5..18 of the high word of the doubleword */

                *hi = (*hi & ~(0x3FFFu << 5)) | ((((*hi >> 5) & 0x3FFF) + delta) & 0x3FFF) << 5;
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/common/dbs", dbsRelocateFileZero__F10_vramAddrsb);
#endif
