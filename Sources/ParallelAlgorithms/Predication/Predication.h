#pragma once

#ifndef OpenCL
#define __local
#define __global
#endif

//If the bit at the provided cl_int matches compared with, the predicate buffer at n is set to 1. 0 otherwise.
inline void BitPredicate(
	__global cl_int *inputBuffer, 
	__global cl_int *predicateBuffer, 
	cl_int index, 
	cl_int comparedWith, 
	cl_int gid)
{
	cl_int self = inputBuffer[gid];
	cl_int x = (((self & (1 << index)) >> index) == comparedWith);
	predicateBuffer[gid] = x;
}

inline void BitPredicateULL(
	__global unsigned long long *inputBuffer, 
	__global cl_int *predicateBuffer, 
	cl_int index, 
	cl_int comparedWith, 
	cl_int gid)
{
	unsigned long long self = inputBuffer[gid];
	cl_int x = (((self & (1UL << index)) >> index) == comparedWith);
	predicateBuffer[gid] = x;
}

#ifndef OpenCL
#undef __local
#undef __global
#endif
