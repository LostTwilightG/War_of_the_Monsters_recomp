#ifndef COMMON_H
#define COMMON_H

#define INCLUDE_ASM_USE_MACRO_INC 1
#include "include_asm.h"

#ifndef NULL
#define NULL 0
#endif

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long u64; /* EE EABI64: long is 64-bit */
typedef signed long s64;

#endif /* COMMON_H */
