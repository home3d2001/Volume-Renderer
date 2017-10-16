#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#ifdef OpenCL 
	#include "./Sources/Dimension/dim.h"
	#include "./Sources/Vector/vec.h"
	#include "./Sources/Quantize/Quantize.h"
#elif defined __cplusplus
extern "C" {
	#include "Dimension/dim.h"
}
	#include "Quantize/Quantize.h"
	#include "Vector/vec.h"
#else 
#error Only C++/OpenCL can compile BoundingBox.h!
#endif

/* ------------------------------------------------------- */
/* ----------------- 2D Bounding Box Code ---------------- */
/* ------------------------------------------------------- */

typedef struct BoundingBox2D {
  float2 minimum;
  float2 maximum;
  cl_int initialized;
  cl_float maxwidth;
	cl_int pad[2];
} BoundingBox2D;

inline float max_in_float2(const float2 *a) {
	float result = a->s0;
	result = (a->s1 > result) ? a->s1 : result;
  return result;
}

inline int max_in_int2(const int2 *a) {
  int result = a->s0;
  result = (a->s1 > result) ? a->s1 : result;
  return result;
}

inline float2 BB2D_size(const BoundingBox2D *bb) {
	return bb->maximum - bb->minimum;
}

inline float BB2D_max_size(const BoundingBox2D *bb) {
	float2 size = BB2D_size(bb);
	return max_in_float2(&size);
}

inline BoundingBox2D BB2D_initialize(const float2* minimum, const float2* maximum) {
    BoundingBox2D bb;
	bb.initialized = true;
	bb.minimum = *minimum;
	bb.maximum = *maximum;
    bb.maxwidth = BB2D_max_size(&bb);
    return bb;
}

inline int BB2D_max_quantized_size(const BoundingBox2D *bb, const int reslnWidth) {
  float bbSize = BB2D_max_size(bb);
  int2 qMax = Quantize2DPoint(&bb->maximum, &bb->minimum, reslnWidth, bbSize);
  return max_in_int2(&qMax) + 1;
}

inline float2 BB2D_center(const BoundingBox2D *bb) {
	return (bb->minimum + bb->maximum) / 2.0F;
}

inline int2 BB2D_quantized_center(const BoundingBox2D *bb, const int reslnWidth) {
  /* There might be a better way to do this... */
  float2 fCenter = (bb->minimum + bb->maximum) / 2.0F;
  float bbSize = BB2D_max_size(bb);
  return QuantizePoint(&fCenter, &bb->minimum, reslnWidth, bbSize);
}

inline bool BB2D_contains_point(const BoundingBox2D *bb, float2 *point, const float epsilon) {
	if ((point->s0 <= bb->minimum.s0 - epsilon) ||
		(point->s0 >= bb->maximum.s0 + epsilon))
		return false;
	if ((point->s1 <= bb->minimum.s1 - epsilon) ||
		(point->s1 >= bb->maximum.s1 + epsilon))
		return false;
	return true;
}

// Returns the smallest square bounding box that contains
// bb and has identical origin.
inline BoundingBox2D BB2D_make_square(const BoundingBox2D *bb) {
	float2 size = BB2D_size(bb);
	float dwidth = max_in_float2(&size);
	BoundingBox2D result = BB2D_initialize(&bb->minimum, &bb->minimum);
	result.maximum += dwidth;
    result.maxwidth = BB2D_max_size(&result);
    return result;
}

inline BoundingBox2D BB2D_make_centered_square(const BoundingBox2D *bb) {
	float2 size = BB2D_size(bb);
	float dwidth = max_in_float2(&size);

	//The resulting box is square, so init both in and max with original min. Then add dwidth to max.
	BoundingBox2D result = BB2D_initialize(&bb->minimum, &bb->minimum);
	result.maximum += dwidth;

	result.minimum.s0 -= (dwidth - size.s0) / 2.0F;
	result.maximum.s0 = result.minimum.s0 + dwidth;

	result.minimum.s1 -= (dwidth - size.s1) / 2.0F;
	result.maximum.s1 = result.minimum.s1 + dwidth;

	result.initialized = true;
    result.maxwidth = BB2D_max_size(&result);
    return result;
}

inline BoundingBox2D BB2D_scale(const BoundingBox2D *bb, const float f) {
	float2 size = BB2D_size(bb);
	size *= f;

	float2 newMax = bb->minimum + size;
	return BB2D_initialize(&bb->minimum, &newMax);
}

inline BoundingBox2D BB2D_scale_centered(BoundingBox2D *result, const BoundingBox2D *bb, const float f) {
	float2 s = BB2D_size(bb);
	s *= f;
	s /= 2.0F;

	float2 c = BB2D_center(bb);

	float2 minp = c - s;
	float2 maxp = c + s;

	return BB2D_initialize(&minp, &maxp);
}

inline bool BB2D_is_square(const BoundingBox2D *bb) {
	float2 s = BB2D_size(bb);
	float a = s.x;
	if (s.s1 != a) return false;
	return true;
}

#ifdef __cplusplus
#include <iostream>
inline std::ostream& operator<<(std::ostream& out, const BoundingBox2D& bb) {
  out << bb.minimum << " " << bb.maximum;
  return out;
}
#endif

/* ------------------------------------------------------- */
/* ----------------- 3D Bounding Box Code ---------------- */
/* ------------------------------------------------------- */

typedef struct BoundingBox3D {
	float3 minimum;
	float3 maximum;
	cl_int initialized;
	cl_float maxwidth;
	cl_int pad[2];
} BoundingBox3D;

inline float max_in_float3(const float3 *a) {
	float result = a->s0;
	result = (a->s1 > result) ? a->s1 : result;
	result = (a->s2 > result) ? a->s2 : result;
	return result;
}

inline int max_in_int3(const int3 *a) {
	int result = a->s0;
	result = (a->s1 > result) ? a->s1 : result;
	result = (a->s2 > result) ? a->s2 : result;
	return result;
}

inline float3 BB3D_size(const BoundingBox3D *bb) {
	return bb->maximum - bb->minimum;
}

inline float BB3D_max_size(const BoundingBox3D *bb) {
	float3 size = BB3D_size(bb);
	return max_in_float3(&size);
}

inline BoundingBox3D BB3D_initialize(const float3* minimum, const float3* maximum) {
	BoundingBox3D bb;
	bb.initialized = true;
	bb.minimum = *minimum;
	bb.maximum = *maximum;
	bb.maxwidth = BB3D_max_size(&bb);
	return bb;
}

inline int BB3D_max_quantized_size(const BoundingBox3D *bb, const int reslnWidth) {
	float bbSize = BB3D_max_size(bb);
	int3 qMax = Quantize3DPoint(&bb->maximum, &bb->minimum, reslnWidth, bbSize);
	return max_in_int3(&qMax) + 1;
}

inline float3 BB3D_center(const BoundingBox3D *bb) {
	return (bb->minimum + bb->maximum) / 2.0F;
}

inline int3 BB3D_quantized_center(const BoundingBox3D *bb, const int reslnWidth) {
	/* There might be a better way to do this... */
	float3 fCenter = (bb->minimum + bb->maximum) / 2.0F;
	float bbSize = BB3D_max_size(bb);
	return Quantize3DPoint(&fCenter, &bb->minimum, reslnWidth, bbSize);
}

inline bool BB3D_contains_point(const BoundingBox3D *bb, float3 *point, const float epsilon) {
	if ((point->s0 <= bb->minimum.s0 - epsilon) ||
		(point->s0 >= bb->maximum.s0 + epsilon))
		return false;
	if ((point->s1 <= bb->minimum.s1 - epsilon) ||
		(point->s1 >= bb->maximum.s1 + epsilon))
		return false;
	if ((point->s2 <= bb->minimum.s2 - epsilon) ||
		(point->s2 >= bb->maximum.s2 + epsilon))
		return false;
	return true;
}

// Returns the smallest square bounding box that contains
// bb and has identical origin.
inline BoundingBox3D BB3D_make_square(const BoundingBox3D *bb) {
	float3 size = BB3D_size(bb);
	float dwidth = max_in_float3(&size);
	BoundingBox3D result = BB3D_initialize(&bb->minimum, &bb->minimum);
	result.maximum += dwidth;
	result.maxwidth = BB3D_max_size(&result);
	return result;
}

inline BoundingBox3D BB3D_make_centered_square(const BoundingBox3D *bb) {
	float3 size = BB3D_size(bb);
	float dwidth = max_in_float3(&size);

	//The resulting box is square, so init both in and max with original min. Then add dwidth to max.
	BoundingBox3D result = BB3D_initialize(&bb->minimum, &bb->minimum);
	result.maximum += dwidth;

	result.minimum.s0 -= (dwidth - size.s0) / 2.0F;
	result.maximum.s0 = result.minimum.s0 + dwidth;

	result.minimum.s1 -= (dwidth - size.s1) / 2.0F;
	result.maximum.s1 = result.minimum.s1 + dwidth;

	result.minimum.s2 -= (dwidth - size.s2) / 2.0F;
	result.maximum.s2 = result.minimum.s2 + dwidth;
	
	result.initialized = true;
	result.maxwidth = BB3D_max_size(&result);
	return result;
}

inline BoundingBox3D BB3D_scale(const BoundingBox3D *bb, const float f) {
	float3 size = BB3D_size(bb);
	size *= f;

	float3 newMax = bb->minimum + size;
	return BB3D_initialize(&bb->minimum, &newMax);
}

inline BoundingBox3D BB3D_scale_centered(BoundingBox3D *result, const BoundingBox3D *bb, const float f) {
	float3 s = BB3D_size(bb);
	s *= f;
	s /= 2.0F;

	float3 c = BB3D_center(bb);

	float3 minp = c - s;
	float3 maxp = c + s;

	return BB3D_initialize(&minp, &maxp);
}

inline bool BB3D_is_square(const BoundingBox3D *bb) {
	float3 s = BB3D_size(bb);
	float a = s.x;
	if (s.s1 != a) return false;
	if (s.s2 != a) return false;
	return true;
}

#ifdef __cplusplus
inline std::ostream& operator<<(std::ostream& out, const BoundingBox3D& bb) {
	out << bb.minimum << " " << bb.maximum;
	return out;
}
#endif

#endif
