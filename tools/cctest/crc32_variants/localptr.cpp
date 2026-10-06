extern unsigned long crc_32_tab[];
#define DO1(buf) crc = tab[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf) DO1(buf); DO1(buf);
#define DO4(buf) DO2(buf); DO2(buf);
#define DO8(buf) DO4(buf); DO4(buf);
unsigned long zipCrc32(unsigned long crc, const unsigned char *buf, long len)
{
    unsigned long *tab = crc_32_tab;
    if (buf == 0) return 0L;
    crc = crc ^ 0xffffffffL;
    while (len >= 8) { DO8(buf); len -= 8; }
    if (len) do { DO1(buf); } while (--len);
    return crc ^ 0xffffffffL;
}
