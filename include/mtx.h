#ifndef __MTX_H__
#define __MTX_H__

#include "types.h"

void MTX44Identity(Mtx44* m);
void MTX44Copy(const Mtx44* src, Mtx44* dst);
void MTX44Concat(const Mtx44* a, const Mtx44* b, Mtx44* ab);
void MTX44MultVec33(const Vec3* vec, const Mtx44* m, Vec3* dst);
void MTX44MultVec(const Mtx44* m, const Vec3* src, Vec3* dst);
void MTX44Perspective(Mtx44* m, const float fovY, const float aspect, const float n, const float f);
void MTX44Trans(Mtx44* m, const float x, const float y, const float z);
void MTX44TransApply(const Mtx44* src, Mtx44* dst, const float x, const float y, const float z);
void MTX44Scale(Mtx44* m, const float x, const float y, const float z);
void MTX44ScaleApply(const Mtx44* src, Mtx44* dst, const float x, const float y, const float z);
void MTX44RotRad(Mtx44* m, const char axis, const float rad);
void MTX44RotTrig(Mtx44* m, char axis, const float sinA, const float cosA);
void MTX44ClearRot(const Mtx44* src, Mtx44* dst);

#endif
