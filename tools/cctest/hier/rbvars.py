import subprocess,sys
base='''struct ReadBuf {
    unsigned char data[0x50000];
    int pos;
    int count;
    int size;
};
'''
V={
'f':'''int readBufEndPut(ReadBuf *rb, int n)
{
    int r = n;
    int avail = rb->size - rb->count;

    if (avail < r)
        r = avail;
    rb->count += r;
    rb->pos = (rb->pos + r) % rb->size;
    return r;
}
''',
'g':'''int readBufEndPut(ReadBuf *rb, int n)
{
    int r = n;
    int avail = rb->size - rb->count;

    if (avail < r)
        r = avail;
    rb->pos = (rb->pos + r) % rb->size;
    rb->count += r;
    return r;
}
''',
}
for k,v in V.items():
    open('tools/cctest/hier/rbv.cpp','w').write(base+v)
    r=subprocess.run(['wsl','-d','Ubuntu','-e','sh','-c',"cd /mnt/c/Users/TwistZero/WoTM && ~/.venvs/wotm/bin/python tools/ccmatch.py tools/cctest/hier/rbv.cpp '-O2 -G8' ee-gcc2.95.2-SN-v2.73a 2>&1 | cut -c1-200"],capture_output=True,text=True)
    print(k,r.stdout.strip())
