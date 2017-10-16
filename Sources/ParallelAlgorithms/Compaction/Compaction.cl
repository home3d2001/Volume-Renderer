#include "ParallelAlgorithms/Compaction/Compaction.h"

__kernel void CompactKernel(
	__global cl_int *inputBuffer,
	__global cl_int *resultBuffer,
	__global cl_int *lPredicateBuffer,
	__global cl_int *leftBuffer,
	cl_int size)
{
	Compact(inputBuffer, resultBuffer, lPredicateBuffer, leftBuffer, size, get_global_id(0));
}

__kernel void CompactULLKernel(
	__global unsigned long long *inputBuffer,
	__global unsigned long long *resultBuffer,
	__global cl_int *lPredicateBuffer,
	__global cl_int *leftBuffer,
	cl_int size)
{
	CompactULL(inputBuffer, resultBuffer, lPredicateBuffer, leftBuffer, size, get_global_id(0));
}