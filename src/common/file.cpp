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

struct sceCdCLOCK {
    unsigned char stat, second, minute, hour, pad, day, month, year;
};

struct sceCdlFILE {
    unsigned int lsn;
    unsigned int size;
    char name[16];
    unsigned char date[8];
    unsigned int flag;
};

extern char cdFileSystemToc[];
extern char gFileName[];
extern char globalTimeString[];
extern char D_00735740[];
extern char D_006F3B68[]; /* "Couldn't find the file %s. TOC at %p\n" */
extern char D_006F3B90[]; /* "\t\tError #%i\n" */
extern char D_006F86A0[]; /* "host0:" */
extern char D_006F86A8[]; /* "cdrom0:" */
extern char D_006F86B0[]; /* "\\" */
extern char D_006F86B8[]; /* ";1" */

/* "\\" plus its terminator, copied as one 2-byte block like retail */
struct CharPair {
    char c[2];
};
extern "C" {
extern const char _ctype_[];
int strncmp(const char *a, const char *b, unsigned int n);
char *strncat(char *dst, const char *src, unsigned int n);
char *strcat(char *dst, const char *src);
int snd_StreamSafeCdSync(int);
int snd_StreamSafeCdGetError(void);
int snd_StreamSafeCdRead(int, int, void *, void *);
unsigned int sceCdGetReadPos(void);
int sceCdRead(unsigned int lsn, unsigned int sectors, void *buf, sceCdRMode *mode);
int sceCdSync(int mode);
int sceCdReadClock(sceCdCLOCK *clock);
int sceOpen(const char *name, int flags, ...);
int sceWrite(int fd, const void *buf, int size);
int sceClose(int fd);
int printf(const char *fmt, ...);
int sprintf(char *buf, const char *fmt, ...);
}
void setMaxTexId(int i, unsigned short v);
void setMaxTexAddr(int i, unsigned short v);
void setMaxResAddr(int i, unsigned short v);
int fileStringCompare(char *a, char *b);
void fileAdjustFileName(char *dst, char *src);
int fileCdSearchFile(sceCdlFILE *f, char *path);

INCLUDE_ASM("asm/nonmatchings/common/file", fileInitializeCd__Fv);
int fileReadf(char *name, void *buf)
{
    sceCdlFILE f;
    unsigned int pos;
    int done;

    fileAdjustFileName(gFileName, name);
    printf("Reading %s into address %p\n", gFileName, buf);
    f.lsn = 0;
    f.size = 0;
    f.name[0] = 0;
    f.date[0] = 0;
    if (!fileCdSearchFile(&f, gFileName)) {
        printf(D_006F3B68, gFileName, cdFileSystemToc);
        return 0;
    }
    snd_StreamSafeCdSync(0);
    do {
        if (snd_StreamSafeCdRead(f.lsn, (f.size + 0x7FF) >> 11, buf, &cdReadMode)) {
            pos = sceCdGetReadPos();
            while (snd_StreamSafeCdSync(0)) {
                if (sceCdGetReadPos() > pos + f.size / 10)
                    pos = sceCdGetReadPos();
            }
            done = 1;
            if (snd_StreamSafeCdGetError()) {
                done = 0;
                printf(D_006F3B90, snd_StreamSafeCdGetError());
            }
        } else
            done = 0;
    } while (!done);
    return f.size;
}
INCLUDE_ASM("asm/nonmatchings/common/file", D_006F3B68);
INCLUDE_ASM("asm/nonmatchings/common/file", D_006F3B90);
INCLUDE_ASM("asm/nonmatchings/common/file", fileReada__FPcPv);
INCLUDE_ASM("asm/nonmatchings/common/file", fileReads__FPcPvUi);
int fileWritef(char *name, void *buf, int size)
{
    int fd;

    fileAdjustFileName(gFileName, name);
    fd = sceOpen(gFileName, 0x602);
    if (fd < 0)
        return -2;
    sceWrite(fd, buf, size);
    sceClose(fd);
    return 0;
}
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
#ifdef NON_MATCHING
/* 38/74 words: same logic; retail keeps two pointers for path[i] (test and copy), gcc merges them here */
char *fileTrimPath(char *path)
{
    int i, j;

    for (i = 0; i < 9; i++)
        D_00735740[i] = 0;
    i = 1;
    while (path[i] != '\\')
        i++;
    if (path[i] == '\\' || path[i] == '/' || path[i] == ':')
        i++;
    j = 0;
    while (path[i] != '.') {
        if (path[i] == '\\') {
            j = 0;
            i++;
        }
        D_00735740[j++] = path[i++];
    }
    D_00735740[j] = 0;
    printf("fileTrimPath passed in \"%s\", returning \"%s\"\n", path, D_00735740);
    return D_00735740;
}
#else
INCLUDE_ASM("asm/nonmatchings/common/file", fileTrimPath__FPc);
#endif
char *fileGetTimeString(void)
{
    sceCdCLOCK clock;

    if (sceCdReadClock(&clock))
        sprintf(globalTimeString, "%02x:%02x:%02x", clock.hour, clock.minute, clock.second);
    else
        sprintf(globalTimeString, "Error Reading Time");
    return globalTimeString;
}
/* retail rodata keeps three empty strings after "Error Reading Time" */
__asm__(".section .rodata
	.word 0
	.word 0
	.word 0
	.text");
void fileAdjustFileName(char *dst, char *src)
{
    char tmp[16];
    int ci;

    if (strncmp(src, D_006F86A0, 6) == 0)
        src += 6;
    else if (strncmp(src, D_006F86A8, 7) == 0)
        src += 7;
    else if (*src == '/' || *src == '\\')
        src++;
    *(CharPair *)dst = *(CharPair *)D_006F86B0;
    while (*src) {
        switch (*src) {
        case ';':
            src++;
            break;
        case '/':
        case '\\':
            strncat(dst, D_006F86B0, 1);
            break;
        case '~':
            src++;
            break;
        default:
            ci = *src;
            {
                int up = ci - 0x20;
                if (!((_ctype_ + 1)[ci] & 2))
                    up = ci;
                tmp[0] = up;
            }
            strncat(dst, tmp, 1);
            break;
        }
        src++;
    }
    strcat(dst, D_006F86B8);
}
