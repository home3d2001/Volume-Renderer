#pragma once

#include "clfw.hpp"
#include "Octree/Quadnode.h"
#include "BigUnsigned/LCP.h"

inline cl_int GetLineLCPs_p(
	cl::Buffer &linesBuffer_i,
	cl_int totalLines,
	cl::Buffer &zpoints_i,
	int bitsPerZPoint,
	cl::Buffer &LineLCPs_o
)
{
	cl::CommandQueue *queue = &CLFW::DefaultQueue;
	cl::Kernel *kernel = &CLFW::Kernels["GetLineLCPKernel"];
	cl_int error = 0;

	cl_int roundSize = CLFW::NextPow2(totalLines);
	error |= CLFW::getBuffer(LineLCPs_o, "LineLCPs", roundSize * sizeof(LCP));
	error |= kernel->setArg(0, linesBuffer_i);
	error |= kernel->setArg(1, zpoints_i);
	error |= kernel->setArg(2, LineLCPs_o);
	error |= kernel->setArg(3, bitsPerZPoint);
	error |= queue->enqueueNDRangeKernel(*kernel, cl::NullRange, cl::NDRange(totalLines), cl::NullRange);

	return error;
}

inline cl_int GetLineLCP_s(
	std::vector<Line> &lines,
	std::vector<big> &zpoints,
	int mbits,
	std::vector<LCP> &LineLCPs
) {
	for (int i = 0; i < lines.size(); ++i) {
		GetLCPFromLine(lines.data(), zpoints.data(), LineLCPs.data(), mbits, i);
	}
	return CL_SUCCESS;
}

inline cl_int InitializeFacetIndices_p(
	cl_int totalFacets,
	cl::Buffer &facetIndices_o
)
{
	cl::CommandQueue &queue = CLFW::DefaultQueue;
	cl::Kernel &kernel = CLFW::Kernels["InitializeFacetIndicesKernel"];
	cl_int error = 0;

	cl_int roundSize = CLFW::NextPow2(totalFacets);
	error |= CLFW::getBuffer(facetIndices_o, "facetIndices", roundSize * sizeof(cl_int));
	error |= kernel.setArg(0, facetIndices_o);
	error |= queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(totalFacets), cl::NullRange);

	return error;
}

inline cl_int LookUpQuadnodeFromLCP_p(
	cl::Buffer &LCPs_i,
	cl_int numLCPs,
	cl::Buffer &quadtree_i,
	cl::Buffer &LCPToQuadnode_o
) {
	cl::CommandQueue *queue = &CLFW::DefaultQueue;
	cl::Kernel *kernel = &CLFW::Kernels["LookUpQuadnodeFromLCPKernel"];
	int roundNumber = CLFW::NextPow2(numLCPs);
	cl_int error = CLFW::getBuffer(
		LCPToQuadnode_o, "LCPToQuadnode", sizeof(cl_int)* (roundNumber));

	error |= kernel->setArg(0, LCPs_i);
	error |= kernel->setArg(1, quadtree_i);
	error |= kernel->setArg(2, LCPToQuadnode_o);
	error |= queue->enqueueNDRangeKernel( //Around 160 microseconds on gtx 1070
		*kernel, cl::NullRange, cl::NDRange(numLCPs), cl::NullRange);
	return error;
}

inline cl_int LookUpQuadnodeFromLCP_s(
	std::vector<LCP> &LCPs_i,
	std::vector<QuadNode> &quadtree_i,
	std::vector<cl_int> &LCPToQuadnode_o
) {
	LCPToQuadnode_o.resize(LCPs_i.size());
	QuadNode root = quadtree_i[0];
	for (int gid = 0; gid < LCPs_i.size(); ++gid) {
		LCP LCP = LCPs_i[gid];
		int index = getQuadNode(LCP.bu, LCP.len, quadtree_i.data(), &root);
		LCPToQuadnode_o[gid] = index;
	}
	return CL_SUCCESS;
}

inline cl_int GetFacetPairs_s(cl_int* FacetToOctree, Pair *facetPairs, cl_int numLines) {
	for (int i = 0; i < numLines; ++i) {
		int leftNeighbor = (i == 0) ? -1 : FacetToOctree[i - 1];
		int rightNeighbor = (i == numLines - 1) ? -1 : FacetToOctree[i + 1];
		int me = FacetToOctree[i];
		//If my left neighbor doesn't go to the same octnode I go to
		if (leftNeighbor != me) {
			//Then I am the first LCP/Facet belonging to my octnode
			facetPairs[me].first = i;
		}
		//If my right neighbor doesn't go the the same octnode I go to
		if (rightNeighbor != me) {
			//Then I am the last LCP/Facet belonging to my octnode
			facetPairs[me].last = i;
		}
	}
	return CL_SUCCESS;
}

inline cl_int GetLCPBounds_p(
	cl::Buffer &orderedNodeIndices_i,
	cl_int numLines,
	cl_int octreeSize,
	cl::Buffer &facetPairs_o
) {
	cl::CommandQueue *queue = &CLFW::DefaultQueue;
	cl::Kernel *kernel = &CLFW::Kernels["GetFacetPairsKernel"];
	int roundNumber = CLFW::NextPow2(octreeSize);
	cl_int error = CLFW::getBuffer(facetPairs_o, "facetPairs", sizeof(Pair)* (roundNumber));
	Pair initialPair = { -1, -1 };
	error |= queue->enqueueFillBuffer<Pair>(facetPairs_o, { initialPair }, 0, sizeof(Pair) * roundNumber);
	error |= kernel->setArg(0, orderedNodeIndices_i);
	error |= kernel->setArg(1, facetPairs_o);
	error |= kernel->setArg(2, numLines);
	error |= queue->enqueueNDRangeKernel(*kernel, cl::NullRange, cl::NDRange(numLines), cl::NullRange);

	return error;
}

inline cl_int GetLCPBounds_s(
	std::vector<cl_int> &facetToOctnode_i,
	cl_int numLines,
	cl_int octreeSize,
	std::vector<Pair> &facetPairs_o
) {
	facetPairs_o.resize(octreeSize, { -1,-1 });
	for (int gid = 0; gid < numLines; ++gid) {
		int leftNeighbor = (gid == 0) ? -1 : facetToOctnode_i[gid - 1];
		int rightNeighbor = (gid == numLines - 1) ? -1 : facetToOctnode_i[gid + 1];
		int me = facetToOctnode_i[gid];
		//If my left neighbor doesn't go to the same octnode I go to
		if (leftNeighbor != me) {
			//Then I am the first LCP/Facet belonging to my octnode
			facetPairs_o[me].first = gid;
		}
		//If my right neighbor doesn't go the the same octnode I go to
		if (rightNeighbor != me) {
			//Then I am the last LCP/Facet belonging to my octnode
			facetPairs_o[me].last = gid;
		}
	}
	return CL_SUCCESS;
}

inline cl_int FindConflictCells_p(
	cl::Buffer octree_i,
	cl::Buffer leaves_i,
	cl_int numLeaves,
	cl::Buffer LCPToLine_i,
	cl::Buffer LCPBounds_i,
	cl::Buffer lines_i,
	cl_int numLines,
	cl::Buffer &qpoints_i,
	cl_int qwidth,
	bool keepCollisions,
	cl::Buffer &conflicts_o
) {
	//Two lines are required for an ambigous cell to appear.
	if (numLines < 2) return CL_INVALID_ARG_SIZE;
	cl::CommandQueue &queue = CLFW::DefaultQueue;
	cl::Kernel &kernel = CLFW::Kernels["FindConflictCellsKernel"];
	cl::Buffer initialConflictsBuffer;
	cl_int error = 0;
	bool isOld;

	/* We need to initialize the conflict info to -1, but only initialize if we're forced to do so. */
	error |= CLFW::getBuffer(conflicts_o, "sparseConflicts", CLFW::NextPow2(numLeaves) * sizeof(Conflict));

	error |= kernel.setArg(0, octree_i);
	error |= kernel.setArg(1, leaves_i);
	error |= kernel.setArg(2, LCPToLine_i);
	error |= kernel.setArg(3, LCPBounds_i);
	error |= kernel.setArg(4, lines_i);
	error |= kernel.setArg(5, numLines);
	error |= kernel.setArg(6, (int)keepCollisions);
	error |= kernel.setArg(7, qpoints_i);
	error |= kernel.setArg(8, qwidth);
	error |= kernel.setArg(9, conflicts_o);
	error |= queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(numLeaves), cl::NullRange);

	return error;
}

inline cl_int FindConflictCells_s(
	std::vector<QuadNode> &octree_i,
	std::vector<Leaf> &leaves_i,
	std::vector<cl_int> &nodeToFacet_i,
	std::vector<Pair> &facetBounds_i,
	std::vector<Line> &lines_i,
	std::vector<intn> &qpoints_i,
	cl_int qwidth,
	bool keepCollisions,
	std::vector<Conflict> &conflicts_o
) {
	conflicts_o.resize(leaves_i.size());

	//Two lines are required for an ambigous cell to appear.
	if (lines_i.size() < 2) return CL_INVALID_ARG_SIZE;

	for (cl_int i = 0; i < leaves_i.size(); ++i) {
		FindConflictCells(
			i, octree_i.data(), leaves_i.data(), nodeToFacet_i.data(), facetBounds_i.data(),
			lines_i.data(), lines_i.size(), keepCollisions, qpoints_i.data(), qwidth, conflicts_o.data());
	}
	return CL_SUCCESS;
}