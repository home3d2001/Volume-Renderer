#include "Vector/vec.h"
#include "BigUnsigned/BigNum.h"
#include "ZOrder/z_order.h"

__kernel void Points2DToMortonKernel(
	__global big *inputBuffer,
	__global int2 *points,
	const cl_int size,
	const cl_int bits
	)
{
	const size_t gid = get_global_id(0);
	const size_t lid = get_local_id(0);
	big tempBU;
	int2 tempPoint = points[gid];

	if (gid < size) {
		xyz2z2D(&tempBU, tempPoint, bits);
	}
	else {
		tempBU = makeBig(0);
	}

	barrier(CLK_GLOBAL_MEM_FENCE);
	inputBuffer[gid] = tempBU;
}


__kernel void Points3DToMortonKernel(
	__global big *inputBuffer,
	__global int3 *points,
	const cl_int size,
	const cl_int bits
	)
{
	const size_t gid = get_global_id(0);
	const size_t lid = get_local_id(0);
	big tempBU;
	int3 tempPoint = points[gid];

	if (gid < size) {
		xyz2z3D(&tempBU, tempPoint, bits);
	}
	else {
		tempBU = makeBig(0);
	}

	barrier(CLK_GLOBAL_MEM_FENCE);
	inputBuffer[gid] = tempBU;
}
