#include "ParallelAlgorithms/Scan/Scan.h"
#pragma OPENCL EXTENSION cl_nv_pragma_unroll: enable

__kernel void StreamScanKernel(
	__global cl_int* buffer,
	__global cl_int* result,
	__global volatile cl_int* I,
	__local cl_int* scratch,
	cl_int totalElements)
{
	const cl_int gid = get_global_id(0);
	const cl_int lid = get_local_id(0);
	const cl_int wid = get_group_id(0);
	const cl_int ls = get_local_size(0);
	cl_int sum = 0;
	if (gid < totalElements) {
		scratch[lid] = buffer[gid];
		if (lid == (ls - 1))
			scratch[ls] = scratch[ls - 1];
	}
	else {
		scratch[lid] = 0;
		if (lid == (ls - 1))
			scratch[ls] = 0;
	}
	barrier(CLK_LOCAL_MEM_FENCE);

	/* Build sum tree */
	for (int s = 1; s <= ls; s <<= 1) {
		int i = (2 * s * (lid + 1)) - 1;
		if (i < ls) {
			scratch[i] += scratch[i - s];
		}
		barrier(CLK_LOCAL_MEM_FENCE);
	}

	//Do Adjacent sync
	if (lid == 0 && gid != 0) {
		while (I[wid - 1] == -1);
		I[wid] = I[wid - 1] + scratch[ls - 1];
	}
	if (gid == 0) I[0] = scratch[ls - 1];

	/* Down-Sweep 4 ways */
	if (lid == 0) scratch[ls - 1] = 0;
	barrier(CLK_LOCAL_MEM_FENCE);

	for (int s = ls / 2; s > 0; s >>= 1) {
		int i = (2 * s * (lid + 1)) - 1;
		int temp;
		if (i < ls)
			temp = scratch[i - s];
		barrier(CLK_LOCAL_MEM_FENCE);
		if (i < ls) {
			scratch[i - s] = scratch[i];
			scratch[i] += temp;
		}
	}
	barrier(CLK_LOCAL_MEM_FENCE);

	if (gid < totalElements) {
		sum = (lid == ls - 1) ? scratch[ls - 1] + scratch[ls] : scratch[lid + 1];
		if (wid != 0) sum += I[wid - 1];
		result[gid] = sum;
	}
}

#define scanBlockSize 1024
	__kernel void NvidiaStreamScanKernel(
		__global cl_int* buffer,
		__global cl_int* result,
		__global volatile int* I,
		__local volatile cl_int* scratch2,
		__local volatile cl_int* scratch)
	{
		const size_t gid = get_global_id(0);
		const size_t lid = get_local_id(0);
		const size_t wid = get_group_id(0);
		const size_t ls = get_local_size(0);
		int sum = 0;
		//FIX THIS
		cl_int n = 1 << 22;


		cl_int tid = get_local_id(0);
		cl_int i = get_group_id(0)*(get_local_size(0) * 2) + get_local_id(0);

		scratch[tid] = (i < n) ? buffer[i] : 0;
		if (tid < 512) {
			// if (i + scanBlockSize < n) 
			//     scratch[tid] += buffer[i+scanBlockSize];  

			// barrier(CLK_LOCAL_MEM_FENCE);

			// // do reduction in shared mem
			// if (scanBlockSize >= 1024) { if (tid < 512) { scratch[tid] += scratch[tid + 512]; } barrier(CLK_LOCAL_MEM_FENCE); }
			// if (scanBlockSize >= 512 ) { if (tid < 256) { scratch[tid] += scratch[tid + 256]; } barrier(CLK_LOCAL_MEM_FENCE); }
			// if (scanBlockSize >= 256 ) { if (tid < 128) { scratch[tid] += scratch[tid + 128]; } barrier(CLK_LOCAL_MEM_FENCE); }
			// if (scanBlockSize >= 128 ) { if (tid <  64) { scratch[tid] += scratch[tid +  64]; } barrier(CLK_LOCAL_MEM_FENCE); }

			// if (tid < 32)
			// {
			//     if (scanBlockSize >=  64) { scratch[tid] += scratch[tid + 32]; }
			//     if (scanBlockSize >=  32) { scratch[tid] += scratch[tid + 16]; }
			//     if (scanBlockSize >=  16) { scratch[tid] += scratch[tid +  8]; }
			//     if (scanBlockSize >=   8) { scratch[tid] += scratch[tid +  4]; }
			//     if (scanBlockSize >=   4) { scratch[tid] += scratch[tid +  2]; }
			//     if (scanBlockSize >=   2) { scratch[tid] += scratch[tid +  1]; }
			// }
		}
		else {
			scratch[lid] = scratch2[lid];
			for (cl_int i = 1; i < ls; i <<= 1) {
				if (lid > (i - 1))
					scratch[lid] = scratch2[lid] + scratch2[lid - i];
				else
					scratch[lid] = scratch2[lid];
				volatile __local cl_int *tmp = scratch;
				scratch = scratch2;
				scratch2 = tmp;
				barrier(CLK_LOCAL_MEM_FENCE);
			}
			sum = scratch2[lid];
		}

		barrier(CLK_LOCAL_MEM_FENCE);
		// if (lid == 0)
		//   result[gid] = scratch[lid];
		// else
		//   result[gid] = 0;
		// int sum = 0;  
		// StreamScan_Init(buffer, scratch2, scratch, gid, lid);
		// barrier(CLK_LOCAL_MEM_FENCE);

		// #pragma unroll 1
		// for (int offset = ls / 2; offset > 32; offset >>= 1) {
		//   AddAll(scratch, lid, offset);
		//   barrier(CLK_LOCAL_MEM_FENCE);
		// }
		// if (lid < 32)
		// {
		//     if (scanBlockSize >=  64) { scratch[lid] += scratch[lid + 32]; }
		//     if (scanBlockSize >=  32) { scratch[lid] += scratch[lid + 16]; }
		//     if (scanBlockSize >=  16) { scratch[lid] += scratch[lid +  8]; }
		//     // if (scanBlockSize >=   8) { scratch[lid] += scratch[lid +  4]; }
		//     // barrier(CLK_LOCAL_MEM_FENCE);
		//     // if (scanBlockSize >=   4) { scratch[lid] += scratch[lid +  2]; }
		//     // barrier(CLK_LOCAL_MEM_FENCE);
		//     // if (scanBlockSize >=   2) { scratch[lid] += scratch[lid +  1]; }
		//     // barrier(CLK_LOCAL_MEM_FENCE);

		// }


		//ADJACENT SYNCRONIZATION
		if (lid == 0 && gid != 0) {
			while (I[wid - 1] == -1);
			I[wid] = I[wid - 1] + scratch[0];
		}
		if (gid == 0) I[0] = scratch[0];

		barrier(CLK_LOCAL_MEM_FENCE);

		result[gid] = scratch2[lid];//scratch[0];



																//   if (wid != 0) sum += I[wid - 1];
																//   result[gid] = sum; 
	}