#ifndef ENGINE_H
#define ENGINE_H

/* Engine-level API shared by common/ and game/: math, timers, animation handles, particles, scene graph.
   Only declarations that more than one translation unit needed live here; signatures are what retail mangles to. */

#include "hieri_types.h"

/* ---- animation ---- */
struct _animHandle {
    int a, b, c, d;
};
void animationGetHandle(_animHandle *h, unsigned a, unsigned b, unsigned c);
void animationStart(_animHandle h, bool loop);
void animationStartReverse(_animHandle h, bool loop);
void animationPause(_animHandle h);
void animationLoop(_animHandle h, bool b);
void animationSetSpeed(_animHandle h, float speed);
void animationSetToBeginning(_animHandle h, bool b);
float animationGetCurrentPercent(_animHandle h);
void animationRunGlobal(void);

/* ---- math ---- */
int mathfRand(int lo, int hi);
float mathfRandf(float lo, float hi);
float mathfHeadingFromPointToPoint(_fvector *from, _fvector *to);
void mathfNormalizeQuaternion(_fvector *dst, _fvector *src);
void mathfQuaternionToMatrix4x4(float (*m)[4], _fvector *q);
void mathfRotAxisToQuaternion(_fvector *dst, _fvector *axis, float angle);
void mathfConcatQuaternions(_fvector *dst, _fvector *a, _fvector *b);
float smoothEasyInTC(float cur, float target, float rate, float eps);

/* ---- input ---- */
void inputUseActuator(int pad, bool enable);

/* ---- timers ---- */
int timerGetFieldsLastFrame(void);
int timerGetUpdateRate(void);

/* ---- particles / scene graph ---- */
void particleKillFx(int &handle);
void hierSetCsEpNode(_cs *cs, _hierhead *ep);
void hdReparentCsGrid(_cs *cs);

#endif
