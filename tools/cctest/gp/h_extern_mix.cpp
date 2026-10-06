extern int ext_small;
extern unsigned long ext_big[4];
int init_before = 5;
int noinit_before;
int f(void) { return ext_small + init_before + noinit_before + (int)ext_big[1] + init_after + noinit_after; }
int init_after = 7;
int noinit_after;
