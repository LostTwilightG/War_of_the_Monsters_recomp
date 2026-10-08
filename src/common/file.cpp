#include "common.h"

#define FILE_MAX_SLOTS 14

// Layout from the accessors below (retail size 0x184).
struct FileStatus {
    char *texAddr[FILE_MAX_SLOTS];        // 0x000
    signed char texLoaded;                // 0x038
    char *ngpAddr[FILE_MAX_SLOTS];        // 0x03C
    signed char ngpLoaded;                // 0x074
    char *resAddr[FILE_MAX_SLOTS];        // 0x078
    signed char resLoaded;                // 0x0B0
    char names[FILE_MAX_SLOTS][9];        // 0x0B1
    unsigned short maxTexId[FILE_MAX_SLOTS];   // 0x130
    unsigned short maxTexAddr[FILE_MAX_SLOTS]; // 0x14C
    unsigned short maxResAddr[FILE_MAX_SLOTS]; // 0x168
};
extern FileStatus fileStatus;
extern unsigned char cdSectorBuffer[];

struct sceCdRMode {
    unsigned char trycount, spindlctrl, datapattern, pad;
};
extern sceCdRMode cdReadMode;

extern "C" {
int sceCdRead(unsigned int lsn, unsigned int sectors, void *buf, sceCdRMode *mode);
int sceCdSync(int mode);
}
void setMaxTexId(int i, unsigned short v);
void setMaxTexAddr(int i, unsigned short v);
void setMaxResAddr(int i, unsigned short v);
int fileStringCompare(char *a, char *b);

INCLUDE_ASM("asm/nonmatchings/common/file", fileInitializeCd__Fv);
INCLUDE_ASM("asm/nonmatchings/common/file", fileReadf__FPcPv);
INCLUDE_ASM("asm/nonmatchings/common/file", D_006F3B68);
INCLUDE_ASM("asm/nonmatchings/common/file", D_006F3B90);
INCLUDE_ASM("asm/nonmatchings/common/file", fileReada__FPcPv);
INCLUDE_ASM("asm/nonmatchings/common/file", fileReads__FPcPvUi);
INCLUDE_ASM("asm/nonmatchings/common/file", fileWritef__FPcPvi);
int getTexFilesLoaded(void)
{
    return fileStatus.texLoaded;
}
int getNgpFilesLoaded(void)
{
    return fileStatus.ngpLoaded;
}
int getResFilesLoaded(void)
{
    return fileStatus.resLoaded;
}
char *getNextNgpLoadAddr(void)
{
    return fileStatus.ngpAddr[fileStatus.ngpLoaded];
}
char *getNextTexLoadAddr(void)
{
    return fileStatus.texAddr[fileStatus.texLoaded];
}
char *getNextResLoadAddr(void)
{
    return fileStatus.resAddr[fileStatus.resLoaded];
}
char *getNgpAddr(int i)
{
    return i < fileStatus.ngpLoaded ? fileStatus.ngpAddr[i] : 0;
}
char *getTexAddr(int i)
{
    return i < fileStatus.texLoaded ? fileStatus.texAddr[i] : 0;
}
char *getResAddr(int i)
{
    return i < fileStatus.resLoaded ? fileStatus.resAddr[i] : 0;
}
char *getGenericName(int i)
{
    return i < fileStatus.ngpLoaded ? fileStatus.names[i] : 0;
}
int getMaxTexId(int i)
{
    return fileStatus.maxTexId[i];
}
int getMaxTexAddr(int i)
{
    return fileStatus.maxTexAddr[i];
}
int getMaxResAddr(int i)
{
    return fileStatus.maxResAddr[i];
}
void setMaxTexId(int i, unsigned short v)
{
    fileStatus.maxTexId[i] = v;
}
void setMaxTexAddr(int i, unsigned short v)
{
    fileStatus.maxTexAddr[i] = v;
}
void setMaxResAddr(int i, unsigned short v)
{
    fileStatus.maxResAddr[i] = v;
}
signed char getIdxOfAddr(char *addr)
{
    int i;

    for (i = 0; i < fileStatus.texLoaded; i++)
        if (fileStatus.texAddr[i] == addr)
            return i;
    return -1;
}
signed char getIdxOfResAddr(char *addr)
{
    int i;

    for (i = 0; i < fileStatus.resLoaded; i++)
        if (fileStatus.resAddr[i] == addr)
            return i;
    return -1;
}
signed char getIdxOfName(char *name)
{
    int i;

    for (i = 0; i < fileStatus.ngpLoaded; i++)
        if (fileStringCompare(name, fileStatus.names[i]))
            return i;
    return -1;
}
void fileAddNgpFile(char *addr, int size)
{
    int rem, pad;

    fileStatus.ngpAddr[fileStatus.ngpLoaded++] = addr;
    fileStatus.ngpAddr[fileStatus.ngpLoaded] = fileStatus.ngpAddr[fileStatus.ngpLoaded - 1] + size;
    rem = (int)fileStatus.ngpAddr[fileStatus.ngpLoaded] & 0x7F;
    if (rem) {
        pad = 0x80 - rem;
        fileStatus.ngpAddr[fileStatus.ngpLoaded] += (signed char)pad;
        fileStatus.ngpAddr[fileStatus.ngpLoaded] += 0x80;
    }
}
void fileAddTexFile(char *addr, int size)
{
    int rem, pad;

    fileStatus.texAddr[fileStatus.texLoaded++] = addr;
    fileStatus.texAddr[fileStatus.texLoaded] = fileStatus.texAddr[fileStatus.texLoaded - 1] + size;
    rem = (int)fileStatus.texAddr[fileStatus.texLoaded] & 0x7F;
    if (rem) {
        pad = 0x80 - rem;
        fileStatus.texAddr[fileStatus.texLoaded] += (signed char)pad;
        fileStatus.texAddr[fileStatus.texLoaded] += 0x70;
    }
}
void fileAddResFile(char *addr, int size)
{
    int rem, pad;

    setMaxResAddr(fileStatus.resLoaded, 0);
    setMaxTexAddr(fileStatus.resLoaded, 0);
    setMaxTexId(fileStatus.resLoaded, 0);
    fileStatus.resAddr[fileStatus.resLoaded++] = addr;
    fileStatus.resAddr[fileStatus.resLoaded] = fileStatus.resAddr[fileStatus.resLoaded - 1] + size;
    rem = (int)fileStatus.resAddr[fileStatus.resLoaded] & 0x7F;
    if (rem) {
        pad = 0x80 - rem;
        fileStatus.resAddr[fileStatus.resLoaded] += (signed char)pad;
        fileStatus.resAddr[fileStatus.resLoaded] += 0x70;
    }
}
void fileAddName(char *name)
{
    int i, j;

    for (i = 0; i < 9; i++)
        fileStatus.names[fileStatus.ngpLoaded - 1][i] = 0;
    for (j = 0; name[j] != 0 && j < 8; j++)
        fileStatus.names[fileStatus.ngpLoaded - 1][j] = name[j];
}
void fileOnlyNgpFile(char *addr, int size)
{
    char *end = addr + size;
    int rem, pad;

    fileStatus.ngpAddr[0] = addr;
    fileStatus.ngpLoaded = 1;
    fileStatus.ngpAddr[1] = end;
    rem = (int)end & 0x7F;
    if (rem) {
        pad = 0x80 - rem;
        fileStatus.ngpAddr[1] = end + (signed char)pad + 0x80;
    }
}
void fileOnlyTexFile(char *addr, int size)
{
    char *end = addr + size;
    int rem, pad;

    fileStatus.texAddr[0] = addr;
    fileStatus.texLoaded = 1;
    fileStatus.texAddr[1] = end;
    rem = (int)end & 0x7F;
    if (rem) {
        pad = 0x80 - rem;
        fileStatus.texAddr[1] = end + (signed char)pad + 0x70;
    }
}
void fileOnlyResFile(char *addr, int size)
{
    char *end = addr + size;
    int rem, pad;

    fileStatus.resAddr[0] = addr;
    fileStatus.resLoaded = 1;
    fileStatus.resAddr[1] = end;
    rem = (int)end & 0x7F;
    if (rem) {
        pad = 0x80 - rem;
        fileStatus.resAddr[1] = end + (signed char)pad + 0x70;
    }
}
void fileInitBeforeDbLoad(void)
{
    int i;

    for (i = 0; i < FILE_MAX_SLOTS; i++) {
        fileStatus.maxTexId[i] = 0;
        fileStatus.maxTexAddr[i] = 0;
        fileStatus.maxResAddr[i] = 0;
    }
}
void filePrintFileStatus(void)
{
}
INCLUDE_ASM("asm/nonmatchings/common/file", fileMakeDirTree__Fv);
int fourCharsToInt(int i)
{
    return cdSectorBuffer[i] + (cdSectorBuffer[i + 1] << 8) + (cdSectorBuffer[i + 2] << 16) + (cdSectorBuffer[i + 3] << 24);
}
short twoCharsToShort(int i)
{
    return cdSectorBuffer[i] + (cdSectorBuffer[i + 1] << 8);
}
int fileCdRead(long sectors, long lsn, char *buf)
{
    int ret = sceCdRead(lsn, sectors, buf, &cdReadMode);

    sceCdSync(0);
    return ret;
}
INCLUDE_ASM("asm/nonmatchings/common/file", fileCdSearchFile__FP10sceCdlFILEPc);
INCLUDE_ASM("asm/nonmatchings/common/file", fileHierAddrOfSect__FUi);
INCLUDE_ASM("asm/nonmatchings/common/file", filePrintCdFiles__FP13_cdFileSystemi);
int fileStringCompare(char *a, char *b)
{
    int i;

    for (i = 0; i < 16; i++) {
        if (a[i] != b[i])
            return 0;
        if (a[i] == 0)
            return 1;
    }
    return 0;
}
INCLUDE_ASM("asm/nonmatchings/common/file", fileTrimPath__FPc);
INCLUDE_ASM("asm/nonmatchings/common/file", fileGetTimeString__Fv);
INCLUDE_ASM("asm/nonmatchings/common/file", fileAdjustFileName__FPcT0);
