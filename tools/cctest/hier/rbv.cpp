struct ReadBuf {
    unsigned char data[0x50000];
    int pos;
    int count;
    int size;
};
int readBufEndPut(ReadBuf *rb, int n)
{
    int r = n;
    int avail = rb->size - rb->count;

    if (avail < r)
        r = avail;
    rb->pos = (rb->pos + r) % rb->size;
    rb->count += r;
    return r;
}
