#include "BigUnsigned/LCP.h"
#include "CellResolution/ConflictCellDetection.h"

__kernel void GetLineLCPKernel(
	__global Line* lines,
	__global big* zpoints,
	__global LCP* LineLCPs,
	const int mbits
	) {
	const int gid = get_global_id(0);
	GetLCPFromLine(lines, zpoints, LineLCPs, mbits, gid);
}

__kernel void InitializeFacetIndicesKernel(
	__global int* facetIndices
	) {
	facetIndices[get_global_id(0)] = get_global_id(0);
}

__kernel void LookUpQuadnodeFromLCPKernel(
	__global LCP* LCPs,
	__global QuadNode* quadtree,
	__global int* FacetToQuadtree
	) {
	const int gid = get_global_id(0);
	LCP LCP = LCPs[gid];
	__local QuadNode root;
	if (get_local_id(0) == 0) {
		root = quadtree[0];
	}
	barrier(CLK_LOCAL_MEM_FENCE);
	int index = getQuadNode(LCP.bu, LCP.len, quadtree, &root);

	FacetToQuadtree[gid] = index;
}

__kernel void GetFacetPairsKernel(
	__global int* FacetToOctree,
	__global Pair *facetPairs,
	int numLines
	)
{
	const int gid = get_global_id(0);
	int leftNeighbor = (gid == 0) ? -1 : FacetToOctree[gid - 1];
	int rightNeighbor = (gid == numLines - 1) ? -1 : FacetToOctree[gid + 1];
	int me = FacetToOctree[gid];
	//If my left neighbor doesn't go to the same octnode I go to
	if (leftNeighbor != me) {
		//Then I am the first LCP/Facet belonging to my octnode
		facetPairs[me].first = gid;
	}
	//If my right neighbor doesn't go the the same octnode I go to
	if (rightNeighbor != me) {
		//Then I am the last LCP/Facet belonging to my octnode
		facetPairs[me].last = gid;
	}
}

__kernel void FindConflictCellsKernel(
	__global QuadNode *quadtree,
	__global Leaf *leaves,
	__global int* nodeToFacet,
	__global Pair *facetBounds,
	__global Line* lines,
	cl_int numLines,
	cl_int keepCollisions,
	__global intn* qpoints,
	cl_int qwidth,
	__global Conflict* conflicts
	) {
	const int gid = get_global_id(0);
	FindConflictCells(
		gid, quadtree, leaves, nodeToFacet, facetBounds,
		lines, numLines, keepCollisions, qpoints, qwidth, conflicts);
}