void fileReads(char *name, void *buf, unsigned int block);
int whichHalfMeg = 0; char *G_FileName = 0; extern unsigned char *zipFileBuf;
#define ZIP_BUF ((unsigned char *)0x01F7F840)
#define ZIP_BUF_END (ZIP_BUF + 0x80000)
unsigned char zipGetChar(void) {
    if (zipFileBuf >= ZIP_BUF_END) { fileReads(G_FileName, ZIP_BUF, whichHalfMeg); whichHalfMeg++; zipFileBuf = ZIP_BUF; }
    return *zipFileBuf++; }
unsigned char *zipFileBuf;
