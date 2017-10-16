#include "ParallelAlgorithms/Predication/Predication.h"

__kernel void PredicateBitKernel(
	__global cl_int *inputBuffer,
	__global cl_int *predicateBuffer,
	cl_int index,
	cl_int comparedWith)
{
	BitPredicate(inputBuffer, predicateBuffer, index, comparedWith, get_global_id(0));
}

__kernel void PredicateULLBitKernel(
	__global unsigned long long *inputBuffer,
	__global cl_int *predicateBuffer,
	cl_int index,
	cl_int comparedWith)
{
	BitPredicateULL(inputBuffer, predicateBuffer, index, comparedWith, get_global_id(0));
}