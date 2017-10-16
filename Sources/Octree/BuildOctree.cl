#include "Octree/BuildOctree.h"

__kernel void ComputeLocalSplitsKernel(
	__global cl_int* local_splits,
	__global BrtNode* I,
	cl_int colored,
	cl_int twoD,
	__global cl_int* colors,
	const int size
	)
{
	cl_int dim = (twoD) ? DIM2D : DIM3D;
	const size_t gid = get_global_id(0);
	if (size > 0 && gid == 0) {
		local_splits[0] = 1 + I[0].lcp.len / dim;
	}
	if (gid < size - 1) {
		ComputeLocalSplits(local_splits, I, colored, twoD, colors, gid);
	}
}

__kernel void BRT2QuadtreeKernel_init(
	__global QuadNode *quadtree
	) {
	brt2quadtree_init(quadtree, get_global_id(0));
}

__kernel void BRT2OctreeKernel_init(
	__global OctNode *octree
	) {
	brt2octree_init(octree, get_global_id(0));
}

__kernel void BRT2QuadtreeKernel(
	__global BrtNode *I,
	const cl_int totalBrtNodes,
	__global QuadNode *quadtree,
	const cl_int totalQuadNodes,
	__global cl_int *localSplits,
	__global cl_int *prefixSums,
	__global cl_int *flags
	) {
	const int gid = get_global_id(0);
	brt2quadtree(I, totalBrtNodes, quadtree, totalQuadNodes, localSplits, prefixSums, flags, gid);
}

__kernel void BRT2OctreeKernel(
	__global BrtNode *I,
	const cl_int totalBrtNodes,
	__global OctNode *octree,
	const cl_int totalOctNodes,
	__global cl_int *localSplits,
	__global cl_int *prefixSums,
	__global cl_int *flags
	) {
	const int gid = get_global_id(0);
	brt2octree(I, totalBrtNodes, octree, totalOctNodes, localSplits, prefixSums, flags, gid);
}

__kernel void ComputeQuadLeavesKernel(
	__global QuadNode *quadtree,
	__global Leaf *leaves,
	__global cl_int *leafPredicates,
	int quadtreeSize
	)
{
	const int gid = get_global_id(0);
	ComputeQuadLeaves(quadtree, leaves, leafPredicates, quadtreeSize, gid);
}

__kernel void ComputeOctLeavesKernel(
	__global OctNode *octree,
	__global Leaf *leaves,
	__global cl_int *leafPredicates,
	int octreeSize
	)
{
	const int gid = get_global_id(0);
	ComputeOctLeaves(octree, leaves, leafPredicates, octreeSize, gid);
}

__kernel void PredicateDuplicateQuadNodesKernelPart1(
	__global QuadNode *origQT,
	__global QuadNode *newQT,
	__global cl_int *predicates,
	int newQTSize
	)
{
	const int gid = get_global_id(0);
	if (gid < newQTSize) {
		PredicateDuplicateQuadNodes(origQT, newQT, predicates, newQTSize, gid);
	}
}

__kernel void PredicateDuplicateQuadNodesKernelPart2(
	__global QuadNode *origQT,
	__global QuadNode *newQT,
	__global cl_int *predicates,
	int newQTSize
	)
{
	//If the current node is a duplicate...
	const int gid = get_global_id(0);
	if (predicates[gid] != -1) {
		//Update my children's "parent" index to the original node
		QuadNode current = origQT[gid];
		for (int i = 0; i < DIM2D; ++i) {
			if ((current.leaf & (1 << i)) == 0)
			{
				//When I perish, have the original node adopt my children. ;(
				newQT[current.children[i]].parent = predicates[gid];
			}
		}
		predicates[gid] = 0; // Predicate me for deletion
	}
	else {
		predicates[gid] = 1; // Compact me to the left.
	}
}