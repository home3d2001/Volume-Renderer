/* Kernels defined in BuildOctree.cl */
#pragma once

#include "clfw.hpp"
#include "ParallelAlgorithms/Scan/Scan.hpp"
#include "ParallelAlgorithms/Compaction/Compaction.hpp"
#include "OctreeDefinitions/defs.h"
#include "Octree/BuildOctree.h"

#include <vector>

inline cl_int ComputeLocalSplits_p(
	cl::Buffer &internalBRTNodes_i,
	cl_int totalBRT,
	cl_int colored,
	bool twoD,
	cl::Buffer &colors,
	std::string uniqueString,
	cl::Buffer &localSplits_o) {
	cl_int globalSize = CLFW::NextPow2(totalBRT);
	cl::Kernel &kernel = CLFW::Kernels["ComputeLocalSplitsKernel"];
	cl::CommandQueue &queue = CLFW::DefaultQueue;

	bool isOld;
	cl::Buffer zeroBuffer;

	cl_int error = CLFW::getBuffer(localSplits_o, uniqueString + "localSplits", sizeof(cl_int) * globalSize);
	error |= CLFW::getBuffer(zeroBuffer, uniqueString + "zeroBuffer", sizeof(cl_int) * globalSize, isOld);

	//Fill any new zero buffers with zero. Then initialize localSplits with zero.
	if (!isOld) {
		cl_int zero = 0;
		error |= queue.enqueueFillBuffer<cl_int>(zeroBuffer, { zero }, 0, sizeof(cl_int) * globalSize);
	}
	error |= queue.enqueueCopyBuffer(zeroBuffer, localSplits_o, 0, 0, sizeof(cl_int) * globalSize);

	error |= kernel.setArg(0, localSplits_o);
	error |= kernel.setArg(1, internalBRTNodes_i);
	error |= kernel.setArg(2, colored);
	error |= kernel.setArg<cl_int>(3, twoD);
	error |= kernel.setArg(4, colors);
	error |= kernel.setArg(5, totalBRT);

	error = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(globalSize), cl::NullRange);
	return error;
}

inline cl_int ComputeLocalSplits_s(
	std::vector<BrtNode> &I,
	bool colored,
	bool twoD, 
	std::vector<cl_int> colors,
	std::vector<cl_int> &local_splits,
	const cl_int size)
{
	cl_int dim = (twoD) ? DIM2D : DIM3D;
	if (size > 0) {
		local_splits[0] = 1 + I[0].lcp.len / dim;
	}
	for (int i = 0; i < size - 1; ++i) {
		ComputeLocalSplits(local_splits.data(), I.data(), colored, twoD, colors.data(), i);
	}
	return CL_SUCCESS;
}

inline cl_int InitQuadtree(
	cl::Buffer &quadtree_i, 
	cl_int quadtreeSize) {
	cl::Kernel &kernel = CLFW::Kernels["BRT2QuadtreeKernel_init"];
	cl::CommandQueue &queue = CLFW::DefaultQueue;
	cl_int error = 0;
	error |= kernel.setArg(0, quadtree_i);
	error |= queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(quadtreeSize), cl::NullRange);
	return error;
}

inline cl_int InitOctree(
	cl::Buffer &octree_i,
	cl_int octreeSize) {
	cl::Kernel &kernel = CLFW::Kernels["BRT2OctreeKernel_init"];
	cl::CommandQueue &queue = CLFW::DefaultQueue;
	cl_int error = 0;
	error |= kernel.setArg(0, octree_i);
	error |= queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(octreeSize), cl::NullRange);
	return error;
}

inline cl_int BinaryRadixToQuadtree_p(
	cl::Buffer &brt_i,
	bool colored,
	cl::Buffer colors_i,
	cl_int totalBRTNode,
	std::string uniqueString,
	cl::Buffer &quadtree_o,
	int &quadtreeSize_o
) {
	cl_int error = 0;

	if (totalBRTNode < 1) return CL_SUCCESS;
	else if (totalBRTNode == 1) {
		error |= CLFW::getBuffer(quadtree_o, uniqueString + "quadtree", sizeof(QuadNode));
		QuadNode root = { -1, -1, -1, -1, -1, -1 - 1, -1, -1 };
		error |= CLFW::Upload<QuadNode>(root, 0, quadtree_o);
		quadtreeSize_o = 1;
		return error;
	}
	int globalSize = CLFW::NextPow2(totalBRTNode);
	cl::Kernel &kernel = CLFW::Kernels["BRT2QuadtreeKernel"];
	cl::CommandQueue &queue = CLFW::DefaultQueue;
	cl::Buffer localSplits, scannedSplits, flags;
	bool isOld;
	error |= CLFW::getBuffer(scannedSplits, uniqueString + "scannedSplits", sizeof(cl_int) * globalSize);
	error |= CLFW::getBuffer(flags, uniqueString + "flags", CLFW::NextPow2(totalBRTNode) * sizeof(cl_int), isOld);
	if (isOld) error |= CLFW::DefaultQueue.enqueueFillBuffer<cl_int>(flags, { 0 }, 0, sizeof(cl_int) * CLFW::NextPow2(totalBRTNode));

	error |= ComputeLocalSplits_p(brt_i, totalBRTNode, colored, true, colors_i, uniqueString, localSplits);

	error |= StreamScan_p(localSplits, globalSize, uniqueString + "quadtreeI", scannedSplits);
	//Read in the required quadtree size
	cl_int quadtreeSize;
	error |= CLFW::DefaultQueue.enqueueReadBuffer(scannedSplits, CL_TRUE,
		sizeof(cl_int)*(totalBRTNode - 1), sizeof(cl_int), &quadtreeSize);
	cl_int roundQuadtreeSize = CLFW::NextPow2(quadtreeSize);

	//Create a quadtree buffer.
	error |= CLFW::getBuffer(quadtree_o, uniqueString + "quadtree", sizeof(QuadNode) * roundQuadtreeSize);

	//use the scanned splits & brt to create quadtree.
	error |= InitQuadtree(quadtree_o, quadtreeSize);
	error |= kernel.setArg(0, brt_i);
	error |= kernel.setArg(1, totalBRTNode);
	error |= kernel.setArg(2, quadtree_o);
	error |= kernel.setArg(3, totalBRTNode);
	error |= kernel.setArg(4, localSplits);
	error |= kernel.setArg(5, scannedSplits);
	error |= kernel.setArg(6, flags);

	error |= queue.enqueueNDRangeKernel(kernel, cl::NDRange(0), cl::NDRange(totalBRTNode), cl::NullRange);
	quadtreeSize_o = quadtreeSize;
	return error;
}

inline cl_int BinaryRadixToOctree_p(
	cl::Buffer &brt_i,
	bool colored,
	cl::Buffer colors_i,
	cl_int totalBRTNode,
	std::string uniqueString,
	cl::Buffer &octree_o,
	int &octreeSize_o
) {
	cl_int error = 0;

	if (totalBRTNode < 1) return CL_SUCCESS;
	else if (totalBRTNode == 1) {
		error |= CLFW::getBuffer(octree_o, uniqueString + "octree", sizeof(OctNode));
		OctNode root = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 - 1, -1, -1 };
		error |= CLFW::Upload<OctNode>(root, 0, octree_o);
		octreeSize_o = 1;
		return error;
	}
	int globalSize = CLFW::NextPow2(totalBRTNode);
	cl::Kernel &kernel = CLFW::Kernels["BRT2OctreeKernel"];
	cl::CommandQueue &queue = CLFW::DefaultQueue;
	cl::Buffer localSplits, scannedSplits, flags;
	bool isOld;
	error |= CLFW::getBuffer(scannedSplits, uniqueString + "scannedSplits", sizeof(cl_int) * globalSize);
	error |= CLFW::getBuffer(flags, uniqueString + "flags", CLFW::NextPow2(totalBRTNode) * sizeof(cl_int), isOld);
	if (isOld) error |= CLFW::DefaultQueue.enqueueFillBuffer<cl_int>(flags, { 0 }, 0, sizeof(cl_int) * CLFW::NextPow2(totalBRTNode));

	error |= ComputeLocalSplits_p(brt_i, totalBRTNode, colored, false, colors_i, uniqueString, localSplits);

	error |= StreamScan_p(localSplits, globalSize, uniqueString + "octreeI", scannedSplits);
	//Read in the required octree size
	cl_int octreeSize;
	error |= CLFW::DefaultQueue.enqueueReadBuffer(scannedSplits, CL_TRUE,
		sizeof(cl_int)*(totalBRTNode - 1), sizeof(cl_int), &octreeSize);
	cl_int roundOctreeSize = CLFW::NextPow2(octreeSize);

	//Create a octree buffer.
	error |= CLFW::getBuffer(octree_o, uniqueString + "octree", sizeof(OctNode) * roundOctreeSize);

	//use the scanned splits & brt to create octree.
	error |= InitOctree(octree_o, octreeSize);
	error |= kernel.setArg(0, brt_i);
	error |= kernel.setArg(1, totalBRTNode);
	error |= kernel.setArg(2, octree_o);
	error |= kernel.setArg(3, totalBRTNode);
	error |= kernel.setArg(4, localSplits);
	error |= kernel.setArg(5, scannedSplits);
	error |= kernel.setArg(6, flags);

	error |= queue.enqueueNDRangeKernel(kernel, cl::NDRange(0), cl::NDRange(totalBRTNode), cl::NullRange);
	octreeSize_o = octreeSize;
	return error;
}

inline cl_int BinaryRadixToQuadtree_s(
	std::vector<BrtNode> &internalBRTNodes_i,
	bool colored,
	std::vector<cl_int> brtColors_i,
	std::vector<QuadNode> &quadtree_o
) {
	int size = internalBRTNodes_i.size();
	std::vector<cl_int> localSplits(size);
	ComputeLocalSplits_s(internalBRTNodes_i, colored, true, brtColors_i, localSplits, size);

	std::vector<cl_int> prefixSums(size);
	StreamScan_s(localSplits, prefixSums);

	std::vector<cl_int> flags(internalBRTNodes_i.size(), 0);

	const int quadtreeSize = prefixSums[size - 1];
	quadtree_o.resize(quadtreeSize);
	quadtree_o[0].parent = -1;
	quadtree_o[0].level = 0;
	for (int i = 0; i < quadtreeSize; ++i)
		brt2quadtree_init(quadtree_o.data(), i);
	for (int brt_i = 0; brt_i < size; ++brt_i)
		brt2quadtree(internalBRTNodes_i.data(), internalBRTNodes_i.size(),
			quadtree_o.data(), quadtree_o.size(), localSplits.data(),
			prefixSums.data(), flags.data(), brt_i);
	return CL_SUCCESS;
}

inline cl_int BinaryRadixToOctree_s(
	std::vector<BrtNode> &internalBRTNodes_i,
	bool colored,
	std::vector<cl_int> brtColors_i,
	std::vector<OctNode> &octree_o
) {
	int size = internalBRTNodes_i.size();
	std::vector<cl_int> localSplits(size);
	ComputeLocalSplits_s(internalBRTNodes_i, colored, false, brtColors_i, localSplits, size);

	std::vector<cl_int> prefixSums(size);
	StreamScan_s(localSplits, prefixSums);

	std::vector<cl_int> flags(internalBRTNodes_i.size(), 0);

	const int octreeSize = prefixSums[size - 1];
	octree_o.resize(octreeSize);
	octree_o[0].parent = -1;
	octree_o[0].level = 0;
	for (int i = 0; i < octreeSize; ++i)
		brt2octree_init(octree_o.data(), i);
	for (int brt_i = 0; brt_i < size; ++brt_i)
		brt2octree(internalBRTNodes_i.data(), internalBRTNodes_i.size(),
			octree_o.data(), octree_o.size(), localSplits.data(),
			prefixSums.data(), flags.data(), brt_i);
	return CL_SUCCESS;
}

inline cl_int GenerateQuadLeaves_p(
	cl::Buffer &quadtree_i,
	int quadtreeSize,
	cl::Buffer &sparseleaves_o,
	cl::Buffer &leafPredicates_o)
{
	cl_int error = 0;
	error |= CLFW::getBuffer(sparseleaves_o, "sleaves", CLFW::NextPow2(4 * quadtreeSize) * sizeof(Leaf));
	error |= CLFW::getBuffer(leafPredicates_o, "lfPrdcts", CLFW::NextPow2(4 * quadtreeSize) * sizeof(cl_int));
	cl::Kernel &kernel = CLFW::Kernels["ComputeQuadLeavesKernel"];

	error |= kernel.setArg(0, quadtree_i);
	error |= kernel.setArg(1, sparseleaves_o);
	error |= kernel.setArg(2, leafPredicates_o);
	error |= kernel.setArg(3, quadtreeSize);

	error |= CLFW::DefaultQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(4 * quadtreeSize), cl::NullRange);
	return error;
}

inline cl_int GenerateQuadLeaves_s(
	std::vector<QuadNode> &quadtree,
	cl_int quadtreeSize,
	std::vector<Leaf> &leaves,
	std::vector<cl_int> &leafPredicates
)
{
	leaves.resize(4 * quadtreeSize);
	leafPredicates.resize(4 * quadtreeSize);
	for (int i = 0; i < 4 * quadtreeSize; ++i)
		ComputeQuadLeaves(quadtree.data(), leaves.data(), leafPredicates.data(), quadtreeSize, i);
	return CL_SUCCESS;
}

inline cl_int GenerateOctLeaves_p(
	cl::Buffer &octree_i,
	int octreeSize,
	cl::Buffer &sparseleaves_o,
	cl::Buffer &leafPredicates_o)
{
	cl_int error = 0;
	error |= CLFW::getBuffer(sparseleaves_o, "sleaves", CLFW::NextPow2(8 * octreeSize) * sizeof(Leaf));
	error |= CLFW::getBuffer(leafPredicates_o, "lfPrdcts", CLFW::NextPow2(8 * octreeSize) * sizeof(cl_int));
	cl::Kernel &kernel = CLFW::Kernels["ComputeOctLeavesKernel"];

	error |= kernel.setArg(0, octree_i);
	error |= kernel.setArg(1, sparseleaves_o);
	error |= kernel.setArg(2, leafPredicates_o);
	error |= kernel.setArg(3, octreeSize);

	error |= CLFW::DefaultQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(8 * octreeSize), cl::NullRange);
	return error;
}

inline cl_int GenerateOctLeaves_s(
	std::vector<OctNode> &octree,
	cl_int octreeSize,
	std::vector<Leaf> &leaves,
	std::vector<cl_int> &leafPredicates
)
{
	leaves.resize(8 * octreeSize);
	leafPredicates.resize(8 * octreeSize);
	for (int i = 0; i < 8 * octreeSize; ++i)
		ComputeOctLeaves(octree.data(), leaves.data(), leafPredicates.data(), octreeSize, i);
	return CL_SUCCESS;
}

inline cl_int GetQuadLeaves_p(
	cl::Buffer &quadtree_i,
	int quadtreeSize,
	cl::Buffer &Leaves_o,
	int &totalLeaves)
{
	cl::Buffer sparseLeafParents, leafPredicates, leafAddresses;

	cl_int error = GenerateQuadLeaves_p(quadtree_i, quadtreeSize, sparseLeafParents, leafPredicates);
	CLFW::getBuffer(leafAddresses, "lfaddrs", CLFW::NextPow2(quadtreeSize * 4) * sizeof(cl_int));
	CLFW::getBuffer(Leaves_o, "leaves", CLFW::NextPow2(quadtreeSize * 4) * sizeof(Leaf));
	error |= StreamScan_p(leafPredicates, CLFW::NextPow2(quadtreeSize * 4), "lfintrmdt", leafAddresses);
	error |= CLFW::DefaultQueue.enqueueReadBuffer(leafAddresses, CL_TRUE, (sizeof(cl_int)*(quadtreeSize * 4) - (sizeof(cl_int))), sizeof(cl_int), &totalLeaves);
	error |= LeafDoubleCompact_p(sparseLeafParents, Leaves_o, leafPredicates, leafAddresses, quadtreeSize * 4);
	return error;
}

inline cl_int GetOctLeaves_p(
	cl::Buffer &octree_i,
	int octreeSize,
	cl::Buffer &Leaves_o,
	int &totalLeaves)
{
	cl::Buffer sparseLeafParents, leafPredicates, leafAddresses;

	cl_int error = GenerateOctLeaves_p(octree_i, octreeSize, sparseLeafParents, leafPredicates);
	CLFW::getBuffer(leafAddresses, "lfaddrs", CLFW::NextPow2(octreeSize * 8) * sizeof(cl_int));
	CLFW::getBuffer(Leaves_o, "leaves", CLFW::NextPow2(octreeSize * 8) * sizeof(Leaf));
	error |= StreamScan_p(leafPredicates, CLFW::NextPow2(octreeSize * 8), "lfintrmdt", leafAddresses);
	error |= CLFW::DefaultQueue.enqueueReadBuffer(leafAddresses, CL_TRUE, (sizeof(cl_int)*(octreeSize * 8) - (sizeof(cl_int))), sizeof(cl_int), &totalLeaves);
	error |= LeafDoubleCompact_p(sparseLeafParents, Leaves_o, leafPredicates, leafAddresses, octreeSize * 8);
	return error;
}

inline cl_int PredicateDuplicateQuadNodes_p(
	cl::Buffer origQT_i, 
	cl::Buffer newQT_i, 
	int newQTSize, 
	cl::Buffer &duplicate_o) 
{
	cl_int error = 0;
	cl::Kernel &kernel = CLFW::Kernels["PredicateDuplicateQuadNodesKernelPart1"];
	CLFW::getBuffer(duplicate_o, "dup", sizeof(cl_int) * CLFW::NextPow2(newQTSize));
	error |= kernel.setArg(0, origQT_i);
	error |= kernel.setArg(1, newQT_i);
	error |= kernel.setArg(2, duplicate_o);
	error |= kernel.setArg(3, newQTSize);
	error |= CLFW::DefaultQueue.enqueueNDRangeKernel(kernel, cl::NullRange, 
		cl::NDRange(CLFW::NextPow2(newQTSize)), cl::NullRange);

	//Nvidia's drivers crash when I run this code in one kernel. 
	cl::Kernel &kernel2 = CLFW::Kernels["PredicateDuplicateQuadNodesKernelPart2"];
	cl_int memsize = kernel2.getWorkGroupInfo<CL_KERNEL_PRIVATE_MEM_SIZE>(CLFW::DefaultDevice);
	error |= kernel2.setArg(0, origQT_i);
	error |= kernel2.setArg(1, newQT_i);
	error |= kernel2.setArg(2, duplicate_o);
	error |= kernel2.setArg(3, newQTSize);
	error |= CLFW::DefaultQueue.enqueueNDRangeKernel(kernel2, cl::NullRange, 
		cl::NDRange(CLFW::NextPow2(newQTSize)), cl::NullRange);
	return error;
}