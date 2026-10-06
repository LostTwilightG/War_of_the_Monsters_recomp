extern unsigned char *zipFileBuf;
extern int whichHalfMeg;
extern char *G_FileName;
void fileReads(char *, void *, unsigned int);
unsigned char zipGetChar(void)
{
    if (zipFileBuf > (unsigned char *)0x1FFF83F) {
        fileReads(G_FileName, (void *)0x1F7F840, whichHalfMeg++);
        zipFileBuf = (unsigned char *)0x1F7F840;
    }
    return *zipFileBuf++;
}
int whichHalfMeg;
char *G_FileName;
unsigned char *zipFileBuf;
