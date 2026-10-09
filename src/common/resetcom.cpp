#include "common.h"

/* A tweak file (AI/monster parameters) that can be re-read or written back at run time. The vtable pointer comes
   last (gcc 2.95 layout); the ctors and the type info stay in asm. */
class resetcom {
public:
    char m_data[0xC000];      /* 0x0000 file contents */
    char m_path[0x40];        /* 0xC000 */
    char m_fileName[0x40];    /* 0xC040 */
    int m_flag;               /* 0xC080 */
    char *m_buf;              /* 0xC084 */
    int m_creatorKey;         /* 0xC088 */
    int m_forceRead;          /* 0xC08C */
    int m_forceWrite;         /* 0xC090 */
    int m_unkC094;            /* 0xC094 */
    void *m_vtable;           /* 0xC098 */

    int writeKeys(char *buf);
    int write(char *buf);
    int readKey(char *buf);
    int read(char *buf);
    int oldStyleRead(char *buf);
    void init(void);
    void update(void);
    void getFileAndPathname(int read);
    int validFileAndExtension(int read);
    void forceRead(void);
    void forceRead(int on);
    void forceWrite(void);
    void forceWrite(int on);
    void getFileName(char **name);
    void setFileName(char *name, bool flag);
};

extern "C" char *strcpy(char *dst, const char *src);
int fileReadf(char *name, void *dest);

INCLUDE_ASM("asm/nonmatchings/common/resetcom", __8resetcom);
INCLUDE_ASM("asm/nonmatchings/common/resetcom", __8resetcom11CREATOR_KEY);
INCLUDE_ASM("asm/nonmatchings/common/resetcom", writeKeys__8resetcomPc);
INCLUDE_ASM("asm/nonmatchings/common/resetcom", write__8resetcomPc);
INCLUDE_ASM("asm/nonmatchings/common/resetcom", readKey__8resetcomPc);
INCLUDE_ASM("asm/nonmatchings/common/resetcom", read__8resetcomPc);
int resetcom::oldStyleRead(char *)
{
    return 0;
}
void resetcom::init(void)
{
}
INCLUDE_ASM("asm/nonmatchings/common/resetcom", update__8resetcom);
INCLUDE_ASM("asm/nonmatchings/common/resetcom", getFileAndPathname__8resetcomi);
int resetcom::validFileAndExtension(int read)
{
    int ok = 1;

    m_buf = m_data;
    getFileAndPathname(read);
    if (read) {
        if (fileReadf(m_path, m_buf) <= 0)
            ok = 0;
    }
    return ok;
}
void resetcom::forceRead(void)
{
    forceRead(1);
}
void resetcom::forceRead(int on)
{
    m_forceRead = on;
    m_forceWrite = 0;
    update();
}
void resetcom::forceWrite(void)
{
    forceWrite(1);
}
void resetcom::forceWrite(int on)
{
    m_forceWrite = on;
    m_forceRead = 0;
    update();
}
void resetcom::getFileName(char **name)
{
    *name = m_fileName;
}
void resetcom::setFileName(char *name, bool flag)
{
    char tmp[64];
    int i, j;

    strcpy(tmp, name);
    /* spaces are dropped */
    j = 0;
    for (i = 0; i < 64; i++)
        if (tmp[i] != ' ')
            m_fileName[j++] = tmp[i];
    m_fileName[j] = 0;
    m_flag = flag;
}
INCLUDE_ASM("asm/nonmatchings/common/resetcom", _vt$8resetcom);
INCLUDE_ASM("asm/nonmatchings/common/resetcom", __tf8resetcom);
