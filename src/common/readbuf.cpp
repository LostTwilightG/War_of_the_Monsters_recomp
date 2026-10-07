#include "common.h"

struct ReadBuf {
    unsigned char data[0x50000];
    int pos;
    int count;
    int size;
};

int readBufCreate(ReadBuf *rb, int size)
{
    rb->size = size;
    rb->pos = 0;
    rb->count = 0;
    return 1;
}
void readBufDelete(ReadBuf *rb)
{
}
int readBufBeginPut(ReadBuf *rb, unsigned char **out)
{
    int avail = rb->size - rb->count;

    if (avail)
        *out = rb->data + rb->pos;
    return avail;
}
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
int readBufBeginGet(ReadBuf *rb, unsigned char **out)
{
    if (rb->count)
        *out = rb->data + (rb->pos - rb->count + rb->size) % rb->size;
    return rb->count;
}
void readBufEndGet(ReadBuf *rb, int n)
{
    rb->count -= rb->count < n ? rb->count : n;
}
