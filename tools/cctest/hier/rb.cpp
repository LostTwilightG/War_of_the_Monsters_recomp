struct ReadBuf {
    unsigned char data[0x50000];
    int pos;
    int count;
    int size;
};
void readBufEndPut(ReadBuf *rb, int n)
{
    int avail = rb->size - rb->count;

    if (avail < n)
        n = avail;
    rb->count += n;
    rb->pos = (rb->pos + n) % rb->size;
}
