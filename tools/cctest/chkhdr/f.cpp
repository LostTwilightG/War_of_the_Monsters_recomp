extern "C" int printf(const char *, ...);
extern unsigned char *zipFileBuf;
#define ZIP_BUF ((unsigned char *)0x01F7F840)
int zipCheckHeader(void)
{
    int sig;
    int ret;
    zipFileBuf = ZIP_BUF;
    sig = *(int *)zipFileBuf;
    if (sig == 0x04034549) {
        unsigned char *size = ZIP_BUF + 22;
        unsigned short *nameLen = (unsigned short *)(ZIP_BUF + 26);
        ret = size[0] + (size[1] << 8) + (size[2] << 16) + (size[3] << 24);
        zipFileBuf = (unsigned char *)(nameLen + 2) + *nameLen;
    } else {
        printf("File signature = 0x%X, should be 0x04034549 (\"IE..\")
", sig);
        ret = 0;
    }
    return ret;
}
unsigned char *zipFileBuf;
