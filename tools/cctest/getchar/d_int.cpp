void fileReads(char *name, void *buf, unsigned int block);
int whichHalfMeg = 0; char *G_FileName = 0; extern unsigned char *zipFileBuf;
#define ZIP_BUF ((unsigned char *)0x01F7F840)
#define ZIP_BUF_END (ZIP_BUF + 0x80000)
unsigned char zipGetChar(void) {
    if ((unsigned int)zipFileBuf >= 0x1FFF840) { fileReads(G_FileName, (void *)0x1F7F840, whichHalfMeg++); zipFileBuf = (unsigned char *)0x1F7F840; }
    return *zipFileBuf++; }
unsigned char *zipFileBuf;
