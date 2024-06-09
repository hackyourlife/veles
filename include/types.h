#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>

#define	u8	uint8_t
#define	u16	uint16_t
#define	u32	uint32_t
#define	u64	uint64_t
#define	s8	int8_t
#define	s16	int16_t
#define	s32	int32_t
#define	s64	int64_t

#define	f32	float
#define	f64	double

typedef	char	bool;
#define	true	1
#define	false	0

#define	BOOL	int
#define	TRUE	1
#define	FALSE	0

typedef struct {
	float x;
	float y;
	float z;
} Vec3;

typedef union {
	struct {
		float _00, _10, _20, _30;
		float _01, _11, _21, _31;
		float _02, _12, _22, _32;
		float _03, _13, _23, _33;
	};
	float m[4][4];
	float a[16];
} Mtx44;

#endif
