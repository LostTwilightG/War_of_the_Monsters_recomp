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
INCLUDE_ASM("asm/nonmatchings/common/file", getIdxOfAddr__FPc);
INCLUDE_ASM("asm/nonmatchings/common/file", getIdxOfResAddr__FPc);
INCLUDE_ASM("asm/nonmatchings/common/file", getIdxOfName__FPc);
INCLUDE_ASM("asm/nonmatchings/common/file", fileAddNgpFile__FPci);
INCLUDE_ASM("asm/nonmatchings/common/file", fileAddTexFile__FPci);
INCLUDE_ASM("asm/nonmatchings/common/file", fileAddResFile__FPci);
INCLUDE_ASM("asm/nonmatchings/common/file", fileAddName__FPc);
INCLUDE_ASM("asm/nonmatchings/common/file", fileOnlyNgpFile__FPci);
INCLUDE_ASM("asm/nonmatchings/common/file", fileOnlyTexFile__FPci);
INCLUDE_ASM("asm/nonmatchings/common/file", fileOnlyResFile__FPci);
INCLUDE_ASM("asm/nonmatchings/common/file", fileInitBeforeDbLoad__Fv);
void filePrintFileStatus(void)
{
}
INCLUDE_ASM("asm/nonmatchings/common/file", fileMakeDirTree__Fv);
INCLUDE_ASM("asm/nonmatchings/common/file", fourCharsToInt__Fi);
INCLUDE_ASM("asm/nonmatchings/common/file", twoCharsToShort__Fi);
INCLUDE_ASM("asm/nonmatchings/common/file", fileCdRead__FllPc);
INCLUDE_ASM("asm/nonmatchings/common/file", fileCdSearchFile__FP10sceCdlFILEPc);
INCLUDE_ASM("asm/nonmatchings/common/file", fileHierAddrOfSect__FUi);
INCLUDE_ASM("asm/nonmatchings/common/file", filePrintCdFiles__FP13_cdFileSystemi);
INCLUDE_ASM("asm/nonmatchings/common/file", fileStringCompare__FPcT0);
INCLUDE_ASM("asm/nonmatchings/common/file", fileTrimPath__FPc);
INCLUDE_ASM("asm/nonmatchings/common/file", fileGetTimeString__Fv);
INCLUDE_ASM("asm/nonmatchings/common/file", fileAdjustFileName__FPcT0);
