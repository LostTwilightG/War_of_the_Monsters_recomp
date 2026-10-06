const unsigned long crc_32_tab[256] = {0x00000000L, 0x77073096L};
#define DO1(buf) crc = crc_32_tab[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf) DO1(buf); DO1(buf);
#define DO4(buf) DO2(buf); DO2(buf);
#define DO8(buf) DO4(buf); DO4(buf);
unsigned long zipCrc32(unsigned long crc, const unsigned char *buf, long len)
{
    if (buf == 0) return 0L;
    crc = crc ^ 0xffffffffL;
    while (len >= 8) { DO8(buf); len -= 8; }
    if (len) do { DO1(buf); } while (--len);
    return crc ^ 0xffffffffL;
}
