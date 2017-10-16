#pragma once

#ifndef OpenCL
#define __local
#define __global
#endif

inline void Compact(
	__global cl_int *inputBuffer, 
	__global cl_int *resultBuffer, 
	__global cl_int *predicationBuffer,
	__global cl_int *addressBuffer, 
	cl_int size, 
	const cl_int gid)
{
	cl_int a = addressBuffer[gid];
	cl_int b = addressBuffer[size - 2];
	cl_int c = predicationBuffer[gid];
	cl_int e = predicationBuffer[size - 1];

	//Check out http://http.developer.nvidia.com/GPUGems3/gpugems3_ch39.html figure 39-14
	cl_int t = gid - a + (e + b);
	cl_int d = (!c) ? t : a - 1;

	//This really suffers from poor coalescing
#ifdef OpenCL
	barrier(CLK_GLOBAL_MEM_FENCE);
#endif
	resultBuffer[d] = inputBuffer[gid];
}

inline void CompactULL(
	__global unsigned long long *inputBuffer,
	__global unsigned long long *resultBuffer, 
	__global cl_int *predicationBuffer,
	__global cl_int *addressBuffer, 
	cl_int size, 
	const cl_int gid)
{
	cl_int a = addressBuffer[gid];
	cl_int b = addressBuffer[size - 2];
	cl_int c = predicationBuffer[gid];
	cl_int e = predicationBuffer[size - 1];

	//Check out http://http.developer.nvidia.com/GPUGems3/gpugems3_ch39.html figure 39-14
	cl_int t = gid - a + (e + b);
	cl_int d = (!c) ? t : a - 1;

#ifdef OpenCL
	barrier(CLK_GLOBAL_MEM_FENCE);
#endif
	resultBuffer[d] = inputBuffer[gid];
}

#ifndef OpenCL
#undef __local
#undef __global
#endif
