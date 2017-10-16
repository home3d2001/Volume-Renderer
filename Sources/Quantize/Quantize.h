#ifndef QUANTIZE_H
#define QUANTIZE_H

#include "Vector/vec.h"

#define IMIN(a,b) ((int)(((a)<(b))?(a):(b)))

inline int2 Quantize2DPoint(const float2 *p, const float2 *minimum, const int reslnWidth, const float bbWidth) {
	int effectiveWidth = reslnWidth;
	float2 d_ = ((*p - *minimum) * (effectiveWidth / bbWidth));
	return make_int2(IMIN(d_.x, reslnWidth - 1), IMIN(d_.y, reslnWidth - 1));
}

inline float2 Unquantize2DPoint(const int2 *p, const float2 *minimum, const int reslnWidth, const float bbWidth) {
	int effectiveWidth = reslnWidth;
	float2 q = make_float2((float)p->x, (float)p->y);
	q = (((q) / (float)effectiveWidth) * bbWidth) + *minimum;
	return q;
}

inline int3 Quantize3DPoint(const float3 *p, const float3 *minimum, const int reslnWidth, const float bbWidth) {
	int effectiveWidth = reslnWidth;
	float3 d_ = ((*p - *minimum) * (effectiveWidth / bbWidth));
	return make_int3(IMIN(d_.x, reslnWidth - 1), IMIN(d_.y, reslnWidth - 1), IMIN(d_.z, reslnWidth - 1));
}

inline float3 Unquantize3DPoint(const int3 *p, const float3 *minimum, const int reslnWidth, const float bbWidth) {
	int effectiveWidth = reslnWidth;
	float3 q = make_float3((float)p->x, (float)p->y, (float)p->z);
	q = (((q) / (float)effectiveWidth) * bbWidth) + *minimum;
	return q;
}

#endif

