#include "ParallelAlgorithms/RadixSort/RadixSort.h"
#pragma OPENCL EXTENSION cl_nv_pragma_unroll: enable

inline int fourWayPrefixSumWithShuffleInternal
(
	__local cl_int *cnt, 
	__local cl_int *offsets, 
	cl_int blkSize, 
	cl_int lid, 
	cl_int extracted
) {
	/* Compute masks */
#pragma unroll
	for (int b = 0; b < 4; ++b) {
		cnt[b * blkSize + lid] = (extracted == b);
	}
	barrier(CLK_LOCAL_MEM_FENCE);

	/* Init BlkSums */
	if (lid == 0) {
#pragma unroll
		for (int b = 0; b < 4; ++b) {
			offsets[b] = cnt[b * blkSize + blkSize - 1];
		}
	}

	/* Build 4 ways sum tree */
	barrier(CLK_LOCAL_MEM_FENCE);
	for (int s = 1; s <= blkSize; s <<= 1) {
		int i = (2 * s * (lid + 1)) - 1;
		if (i < blkSize) {
#pragma unroll
			for (int b = 0; b < 4; ++b) {
				cnt[blkSize * b + i] += cnt[blkSize * b + i - s];
			}
		}
		barrier(CLK_LOCAL_MEM_FENCE);
	}

	/* Down-Sweep 4 ways */
	if (lid == 0)
#pragma unroll
		for (int b = 0; b < 4; ++b)
			cnt[b * blkSize + blkSize - 1] = 0;
	barrier(CLK_LOCAL_MEM_FENCE);
	for (int s = blkSize / 2; s > 0; s >>= 1) {
		int i = (2 * s * (lid + 1)) - 1;
		int temp[4];
		if (i < blkSize)
#pragma unroll
			for (int b = 0; b < 4; ++b)
				temp[b] = cnt[b * blkSize + i - s];
		barrier(CLK_LOCAL_MEM_FENCE);
		if (i < blkSize) {
#pragma unroll
			for (int b = 0; b < 4; ++b) {
				cnt[b * blkSize + i - s] = cnt[b * blkSize + i];
				cnt[b * blkSize + i] += temp[b];
			}
		}
	}
	barrier(CLK_LOCAL_MEM_FENCE);
	/* Get BlkSums */
	if (lid == 0) {
#pragma unroll
		for (int b = 0; b < 4; ++b) {
			offsets[b] += cnt[b * blkSize + blkSize - 1];
		}
	}

	barrier(CLK_LOCAL_MEM_FENCE);
	int offset = 0;
	for (int i = 0; i < extracted; ++i)
		offset += offsets[i];
	return cnt[extracted * blkSize + lid] + offset;
}

__kernel void IntToIntFourWayPrefixSumWithShuffleKernel(
	__global cl_int* keys_i, __global cl_int* vals_i, cl_int bitIndx, cl_int numElems,
	__local cl_int *cnt, __local cl_int *s_keys, __local cl_int *s_vals,
	__global cl_int *blkSum_o, __global cl_int *keyShuffle_o, __global cl_int *valShuffle_o
	) {
	// Initialize
	cl_int gid = get_global_id(0);
	cl_int lid = get_local_id(0);
	cl_int blkSize = get_local_size(0);
	__local cl_int offsets[4];

	// Do 4 way predication
	cl_int key;
	cl_int val;
	if (gid >= numElems && bitIndx == 0) {
		key = 2147483647;
		val = 0;
	}
	else {
		key = keys_i[gid];
		val = vals_i[gid];
	}
	cl_int extracted = (key >> bitIndx) & 3;

	// Perform local shuffle and get local block sum data
	int addr = fourWayPrefixSumWithShuffleInternal(cnt, offsets, blkSize, lid, extracted);
	s_keys[addr] = key;
	s_vals[addr] = val;
	barrier(CLK_LOCAL_MEM_FENCE);
	keyShuffle_o[gid] = s_keys[lid];
	valShuffle_o[gid] = s_vals[lid];
	if (lid < 4) {
		blkSum_o[lid * get_num_groups(0) + get_group_id(0)] = offsets[lid];
	}
}

inline int FourWayMoveElementsInternal
(
	cl_int lid, 
	__local cl_int *counts, 
	__global cl_int *blkSum, 
	__local cl_int *prefixSums, 
	__global cl_int *prefixBlkSum,
	__local cl_int *offsets, 
	cl_int extracted
)
{
	if (lid == 0) {
		for (cl_int b = 0; b < 4; ++b) {
			counts[b] = blkSum[get_num_groups(0) * b + get_group_id(0)];
			prefixSums[b] = (get_group_id(0) == 0 && b == 0) ? 0 : prefixBlkSum[get_num_groups(0) * b + get_group_id(0) - 1];
			offsets[b] = (b == 0) ? 0 : offsets[b - 1] + counts[b - 1];
		}
	}

	barrier(CLK_LOCAL_MEM_FENCE);

	cl_int Pdn = prefixSums[extracted];
	cl_int m = lid - offsets[extracted];
	cl_int a = Pdn + m;

	barrier(CLK_LOCAL_MEM_FENCE);
	return a;
}

__kernel void MoveIntToIntElementsKernel(
	__global cl_int *keyShuffle,
	__global cl_int *valShuffle,
	__local cl_int *s_keys,
	__local cl_int *s_vals,
	__global cl_int *blkSum,
	__global cl_int *prefixBlkSum,
	__global cl_int *keys,
	__global cl_int *vals,
	cl_int bitIndx,
	cl_int numElems
	)
{
	// Initialize
	cl_int gid = get_global_id(0);
	cl_int lid = get_local_id(0);
	__local cl_int offsets[4];
	__local cl_int counts[4];
	__local cl_int prefixSums[4];

	// Get four way predication
	cl_int key = keyShuffle[gid];
	cl_int val = valShuffle[gid];
	cl_int extracted = (key >> bitIndx) & 3;

	// Calculate the result address
	cl_int a = FourWayMoveElementsInternal(lid, counts, blkSum, prefixSums, prefixBlkSum, offsets, extracted);

	// Move the element
	if (a < numElems) {
		keys[a] = key;
		vals[a] = val;
	}
}
