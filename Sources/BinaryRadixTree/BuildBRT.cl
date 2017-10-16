#include "BinaryRadixTree/BuildBRT.h"

__kernel void BuildBinaryRadixTreeKernel(
	__global BrtNode *I,
	__global big* mpoints,
	int mbits,
	int size
	)
{
	BuildBinaryRadixTree(I, nullptr, mpoints, nullptr, mbits, size, false, get_global_id(0));
}

__kernel void BuildColoredBinaryRadixTreeKernel(
	__global BrtNode *I,
	__global cl_int *IColors,
	__global big* mpoints,
	__global cl_int *pointColors,
	int mbits,
	int size
	)
{
	BuildBinaryRadixTree(I, IColors, mpoints, pointColors, mbits, size, true, get_global_id(0));
}

__kernel void PropagateBRTColorsKernel(
	__global BrtNode *brt_i,
	volatile __global cl_int *brtColors_io,
	cl_int totalBrtNodes
	)
{
	cl_int gid = get_global_id(0);
	//for (int gid = 0; gid < totalBrtNodes; gid++) {

	cl_int index = gid;
	BrtNode node = brt_i[gid];

	//Only run BRT nodes with leaves
	if (node.left_leaf || node.right_leaf) {
		cl_int currentColor = brtColors_io[gid];

		//Traverse up the tree
		while (index != 0) {
			index = node.parent;
			node = brt_i[index];

			//If the parent has no color, paint it and exit.
			cl_int r = atomic_cmpxchg(&brtColors_io[index], -1, currentColor);
			if (r == -1)  break;
			// else if our colors don't match, mark it
			else if (r != currentColor) {
				if (r != -2) brtColors_io[index] = -2;
				currentColor = -2;
			}
		}
	}
	//}
}