#include <math.h>
#include "types.h"
#include "mtx.h"

void MTX44Identity(Mtx44* m)
{
	m->_00 = 1.0f; m->_01 = 0.0f; m->_02 = 0.0f; m->_03 = 0.0f;
	m->_10 = 0.0f; m->_11 = 1.0f; m->_12 = 0.0f; m->_13 = 0.0f;
	m->_20 = 0.0f; m->_21 = 0.0f; m->_22 = 1.0f; m->_23 = 0.0f;
	m->_30 = 0.0f; m->_31 = 0.0f; m->_32 = 0.0f; m->_33 = 1.0f;
}

void MTX44Copy(const Mtx44* src, Mtx44* dst)
{
	if(src == dst) {
		return;
	}

	dst->_00 = src->_00; dst->_01 = src->_01; dst->_02 = src->_02; dst->_03 = src->_03;
	dst->_10 = src->_10; dst->_11 = src->_11; dst->_12 = src->_12; dst->_13 = src->_13;
	dst->_20 = src->_20; dst->_21 = src->_21; dst->_22 = src->_22; dst->_23 = src->_23;
	dst->_30 = src->_30; dst->_31 = src->_31; dst->_32 = src->_32; dst->_33 = src->_33;

}
void MTX44Concat(const Mtx44* a, const Mtx44* b, Mtx44* ab)
{
	Mtx44 tmp;
	Mtx44* m;

	if((ab == a) || (ab == b)) {
		m = &tmp;
	} else {
		m = ab;
	}

	/* compute (a x b) -> m */

	m->_00 = a->_00 * b->_00 + a->_01 * b->_10 + a->_02 * b->_20 + a->_03 * b->_30;
	m->_01 = a->_00 * b->_01 + a->_01 * b->_11 + a->_02 * b->_21 + a->_03 * b->_31;
	m->_02 = a->_00 * b->_02 + a->_01 * b->_12 + a->_02 * b->_22 + a->_03 * b->_32;
	m->_03 = a->_00 * b->_03 + a->_01 * b->_13 + a->_02 * b->_23 + a->_03 * b->_33;

	m->_10 = a->_10 * b->_00 + a->_11 * b->_10 + a->_12 * b->_20 + a->_13 * b->_30;
	m->_11 = a->_10 * b->_01 + a->_11 * b->_11 + a->_12 * b->_21 + a->_13 * b->_31;
	m->_12 = a->_10 * b->_02 + a->_11 * b->_12 + a->_12 * b->_22 + a->_13 * b->_32;
	m->_13 = a->_10 * b->_03 + a->_11 * b->_13 + a->_12 * b->_23 + a->_13 * b->_33;

	m->_20 = a->_20 * b->_00 + a->_21 * b->_10 + a->_22 * b->_20 + a->_23 * b->_30;
	m->_21 = a->_20 * b->_01 + a->_21 * b->_11 + a->_22 * b->_21 + a->_23 * b->_31;
	m->_22 = a->_20 * b->_02 + a->_21 * b->_12 + a->_22 * b->_22 + a->_23 * b->_32;
	m->_23 = a->_20 * b->_03 + a->_21 * b->_13 + a->_22 * b->_23 + a->_23 * b->_33;

	m->_30 = a->_30 * b->_00 + a->_31 * b->_10 + a->_32 * b->_20 + a->_33 * b->_30;
	m->_31 = a->_30 * b->_01 + a->_31 * b->_11 + a->_32 * b->_21 + a->_33 * b->_31;
	m->_32 = a->_30 * b->_02 + a->_31 * b->_12 + a->_32 * b->_22 + a->_33 * b->_32;
	m->_33 = a->_30 * b->_03 + a->_31 * b->_13 + a->_32 * b->_23 + a->_33 * b->_33;

	/* overwrite a or b if needed */
	if(m == &tmp) {
		MTX44Copy(&tmp, ab);
	}
}

void MTX44MultVec33(const Vec3* vec, const Mtx44* m, Vec3* dst)
{
	float x = vec->x;
	float y = vec->y;
	float z = vec->z;

	dst->x = x * m->_00 + y * m->_01 + z * m->_02;
	dst->y = x * m->_10 + y * m->_11 + z * m->_12;
	dst->z = x * m->_20 + y * m->_21 + z * m->_22;
}

void MTX44MultVec(const Mtx44* m, const Vec3* src, Vec3* dst)
{
	Vec3 tmp;
	float w;

	/* A Vec3 has a 4th implicit 'w' coordinate of 1 */
	tmp.x = m->_00 * src->x + m->_01 * src->y + m->_02 * src->z + m->_03;
	tmp.y = m->_10 * src->x + m->_11 * src->y + m->_12 * src->z + m->_13;
	tmp.z = m->_20 * src->x + m->_21 * src->y + m->_22 * src->z + m->_23;
	w =     m->_30 * src->x + m->_31 * src->y + m->_32 * src->z + m->_33;
	w = 1.0f / w;

	/* Copy back */
	dst->x = tmp.x * w;
	dst->y = tmp.y * w;
	dst->z = tmp.z * w;
}

void MTX44Perspective(Mtx44* m, const float fovY, const float aspect, const float n, const float f)
{
	float tmp;

	const float angle = fovY / 360.0 * M_PI;
	const float cot = 1.0f / tanf(angle);

	m->_00 = cot / aspect;
	m->_01 = 0.0f;
	m->_02 = 0.0f;
	m->_03 = 0.0f;

	m->_10 = 0.0f;
	m->_11 = cot;
	m->_12 = 0.0f;
	m->_13 = 0.0f;

	m->_20 = 0.0f;
	m->_21 = 0.0f;

	tmp = 1.0f / (f - n);

	m->_22 = -(f + n) * tmp;
	m->_23 = -2.0f * f * n * tmp;

	m->_30 = 0.0f;
	m->_31 = 0.0f;
	m->_32 = -1.0f;
	m->_33 = 0.0f;
}

void MTX44Trans(Mtx44* m, const float x, const float y, const float z)
{
	m->_00 = 1.0f;  m->_01 = 0.0f;  m->_02 = 0.0f;  m->_03 = x;
	m->_10 = 0.0f;  m->_11 = 1.0f;  m->_12 = 0.0f;  m->_13 = y;
	m->_20 = 0.0f;  m->_21 = 0.0f;  m->_22 = 1.0f;  m->_23 = z;
	m->_30 = 0.0f;  m->_31 = 0.0f;  m->_32 = 0.0f;  m->_33 = 1.0f;
}

void MTX44TransApply(const Mtx44* src, Mtx44* dst, const float x, const float y, const float z)
{
	if(src != dst) {
		dst->_00 = src->_00;	dst->_01 = src->_01;	dst->_02 = src->_02;
		dst->_10 = src->_10;	dst->_11 = src->_11;	dst->_12 = src->_12;
		dst->_20 = src->_20;	dst->_21 = src->_21;	dst->_22 = src->_22;
		dst->_30 = src->_30;	dst->_31 = src->_31;	dst->_32 = src->_32;
		dst->_33 = src->_33;
	}

	dst->_03 = src->_03 + x;
	dst->_13 = src->_13 + y;
	dst->_23 = src->_23 + z;
}

void MTX44Scale(Mtx44* m, const float x, const float y, const float z)
{
	m->_00 = x;       m->_01 = 0.0f;  m->_02 = 0.0f;  m->_03 = 0.0f;
	m->_10 = 0.0f;    m->_11 = y;     m->_12 = 0.0f;  m->_13 = 0.0f;
	m->_20 = 0.0f;    m->_21 = 0.0f;  m->_22 = z;     m->_23 = 0.0f;
	m->_30 = 0.0f;    m->_31 = 0.0f;  m->_32 = 0.0f;  m->_33 = 1.0f;
}

void MTX44ScaleApply(const Mtx44* src, Mtx44* dst, const float x, const float y, const float z)
{
	dst->_00 = src->_00 * x;	dst->_01 = src->_01 * x;
	dst->_02 = src->_02 * x;	dst->_03 = src->_03 * x;

	dst->_10 = src->_10 * y;	dst->_11 = src->_11 * y;
	dst->_12 = src->_12 * y;	dst->_13 = src->_13 * y;

	dst->_20 = src->_20 * z;	dst->_21 = src->_21 * z;
	dst->_22 = src->_22 * z;	dst->_23 = src->_23 * z;

	dst->_30 = src->_30;		dst->_31 = src->_31;
	dst->_32 = src->_32;		dst->_33 = src->_33;
}

void MTX44RotRad(Mtx44* m, const char axis, const float rad)
{
	MTX44RotTrig(m, axis, sinf(rad), cosf(rad));
}

void MTX44RotTrig(Mtx44* m, char axis, const float sinA, const float cosA)
{
	axis |= 0x20;
	switch(axis) {
		case 'x':
			m->_00 = 1.0f;  m->_01 = 0.0f;    m->_02 = 0.0f;  m->_03 = 0.0f;
			m->_10 = 0.0f;  m->_11 = cosA;    m->_12 = -sinA; m->_13 = 0.0f;
			m->_20 = 0.0f;  m->_21 = sinA;    m->_22 = cosA;  m->_23 = 0.0f;
			m->_30 = 0.0f;  m->_31 = 0.0f;    m->_32 = 0.0f;  m->_33 = 1.0f;
			break;

		case 'y':
			m->_00 = cosA;  m->_01 = 0.0f;    m->_02 = sinA;  m->_03 = 0.0f;
			m->_10 = 0.0f;  m->_11 = 1.0f;    m->_12 = 0.0f;  m->_13 = 0.0f;
			m->_20 = -sinA; m->_21 = 0.0f;    m->_22 = cosA;  m->_23 = 0.0f;
			m->_30 = 0.0f;  m->_31 = 0.0f;    m->_32 = 0.0f;  m->_33 = 1.0f;
			break;

		case 'z':
			m->_00 = cosA;  m->_01 = -sinA;   m->_02 = 0.0f;  m->_03 = 0.0f;
			m->_10 = sinA;  m->_11 = cosA;    m->_12 = 0.0f;  m->_13 = 0.0f;
			m->_20 = 0.0f;  m->_21 = 0.0f;    m->_22 = 1.0f;  m->_23 = 0.0f;
			m->_30 = 0.0f;  m->_31 = 0.0f;    m->_32 = 0.0f;  m->_33 = 1.0f;
			break;

		default:
			break;
	}
}

void MTX44ClearRot(const Mtx44* src, Mtx44* dst)
{
	dst->_00 = sqrtf(src->_00 * src->_00 + src->_01 * src->_01 + src->_02 * src->_02);
	dst->_01 = 0.0f;
	dst->_02 = 0.0f;
	dst->_03 = src->_03;
	dst->_10 = 0.0f;
	dst->_11 = sqrtf(src->_10 * src->_10 + src->_11 * src->_11 + src->_12 * src->_12);
	dst->_12 = 0.0f;
	dst->_13 = src->_13;
	dst->_20 = 0.0f;
	dst->_21 = 0.0f;
	dst->_22 = sqrtf(src->_20 * src->_20 + src->_21 * src->_21 + src->_22 * src->_22);
	dst->_23 = src->_23;
	dst->_30 = src->_30;
	dst->_31 = src->_31;
	dst->_32 = src->_32;
	dst->_33 = src->_33;
}
