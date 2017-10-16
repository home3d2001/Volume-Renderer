#include "Quantize/Quantize.h"
#include "Vector/vec.h"

__kernel void Quantize2DPointsKernel(
	__global float2 *points,
	__global int2 *quantizePoints,
	const float2 bbMinimum,
	const float bbMaxWidth,
	const int reslnWidth
)
{
	const size_t gid = get_global_id(0);
	const float2 point = points[gid];
	quantizePoints[gid] = Quantize2DPoint(&point, 
		&bbMinimum, reslnWidth, bbMaxWidth);
}

__kernel void Quantize3DPointsKernel(
	__global float3 *points,
	__global int3 *quantizedPoints,
	const float3 bbMinimum,
	const float bbMaxWidth,
	const int reslnWidth
)
{
	const size_t gid = get_global_id(0);
	const float3 point = points[gid];
	quantizedPoints[gid] = Quantize3DPoint(&point,
		&bbMinimum, reslnWidth, bbMaxWidth);
}

__kernel void Unquantize2DPointsKernel(
	__global int2 *quantizedPoints,
	__global float2 *points,
	const float2 bbMinimum,
	const float bbMaxWidth,
	const int reslnWidth
)
{
	const size_t gid = get_global_id(0);
	const int2 qpoint = quantizedPoints[gid];
	points[gid] = Unquantize2DPoint(&qpoint, 
		&bbMinimum, reslnWidth, bbMaxWidth);
}

__kernel void Unquantize3DPointsKernel(
	__global int3 *quantizedPoints,
	__global float3 *points,
	const float3 bbMinimum,
	const float bbMaxWidth,
	const int reslnWidth
)
{
	const size_t gid = get_global_id(0);
	const int3 qpoint = quantizedPoints[gid];
	points[gid] = Unquantize3DPoint(&qpoint,
		&bbMinimum, reslnWidth, bbMaxWidth);
}