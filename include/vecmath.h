#ifndef VECMATH_H
#define VECMATH_H

/* VU0 macro-mode vector helpers. Their asm matches what the retail build inlines everywhere.
   The .set noreorder keeps GNU as from adding the cop1->cop2 hazard nop that ps2eeas leaves out. */

struct _fvector;

static inline void vecAdd(_fvector *dst, _fvector *a, _fvector *b)
{
    __asm__ volatile("lqc2 $vf11, 0x0(%1)\n\t"
                     "lqc2 $vf12, 0x0(%2)\n\t"
                     "vadd.xyz $vf11, $vf11, $vf12\n\t"
                     "sqc2 $vf11, %0"
                     : "=m"(*dst) : "r"(a), "r"(b));
}
static inline void vecSub(_fvector *dst, _fvector *a, _fvector *b)
{
    __asm__ volatile("lqc2 $vf11, 0x0(%1)\n\t"
                     "lqc2 $vf12, 0x0(%2)\n\t"
                     "vsub.xyz $vf11, $vf11, $vf12\n\t"
                     "sqc2 $vf11, %0"
                     : "=m"(*dst) : "r"(a), "r"(b));
}
static inline float vecLenSq(_fvector *v)
{
    float *f = (float *)v;
    register float x __asm__("$f2");
    register float y __asm__("$f1");
    register float z __asm__("$f0");

    __asm__ volatile("lwc1 %0, %3
	"
                     "lwc1 %1, %4
	"
                     "lwc1 %2, %5
	"
                     "mula.s %0, %0
	"
                     "madda.s %1, %1
	"
                     "madd.s %2, %2, %2"
                     : "=f"(x), "=f"(y), "=f"(z) : "m"(f[0]), "m"(f[1]), "m"(f[2]));
    return z;
}
static inline void vecScale(_fvector *dst, _fvector *a, float s)
{
    register int t __asm__("$2");

    __asm__ volatile(".set push\n\t.set noreorder\n\t"
                     "lqc2 $vf11, 0x0(%2)\n\t"
                     "mfc1 %1, %3\n\t"
                     "qmtc2.ni %1, $vf12\n\t"
                     "vmulx.xyz $vf11, $vf11, $vf12x\n\t"
                     "sqc2 $vf11, %0\n\t"
                     ".set pop"
                     : "=m"(*dst), "=r"(t) : "r"(a), "f"(s));
}

#ifdef NON_MATCHING
/* R5900 SQRT.S fd, ft takes its source from ft (fs = 0, e.g. retail 0x46020084). GNU as encodes sqrt.s (and ee-gcc's inline sqrtf) in the
   MIPS32 form with the source in fs, which the EE reads as $f0 (the z term was silently dropped in computeCostEstimate). .word 0x46040104 = sqrt.s $f4,$f4. */
static inline float eeSqrtf(float v)
{
    register float x __asm__("$f4") = v;
    __asm__(".word 0x46040104" : "+f"(x));
    return x;
}
#endif

#endif
