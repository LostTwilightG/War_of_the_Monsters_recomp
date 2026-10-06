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
        ret = ZIP_BUF[22] + (ZIP_BUF[23] << 8) + (ZIP_BUF[24] << 16) + (ZIP_BUF[25] << 24);
        zipFileBuf = ZIP_BUF + 26 + *(unsigned short *)(ZIP_BUF + 26) + 4;
    } else {
        printf("File signature = 0x%X, should be 0x04034549 (\"IE..\")
", sig);
        ret = 0;
    }
    return ret;
}
unsigned char *zipFileBuf;
