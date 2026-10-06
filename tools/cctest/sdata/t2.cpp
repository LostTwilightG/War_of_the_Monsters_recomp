extern unsigned char *outFileBuf;
int whichHalfMeg = 0;
unsigned int G_windowPos = 0;
unsigned int G_bitCount = 0;
unsigned int G_bitBucket = 0;
unsigned long G_crc32val = 0;
struct huft *G_fixedTlen = 0;
char *G_FileName = 0;
void *G_FileAddr = 0;
unsigned char *outFileWindow = outFileBuf;
int tmpLong;
unsigned long G_outSize;
struct huft *G_fixedTdist;
int G_fixedBlen;
int G_fixedBdist;
int howManyBlocks;
unsigned char *zipFileBuf;
unsigned char *outFileBuf;
int f() { return whichHalfMeg; }
