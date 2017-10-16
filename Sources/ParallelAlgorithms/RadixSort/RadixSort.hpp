#pragma once
#include "clfw.hpp"
#include "ParallelAlgorithms/Scan/Scan.hpp"
#include "ParallelAlgorithms/Predication/Predication.hpp"
#include "ParallelAlgorithms/Compaction/Compaction.hpp"
#include "ParallelAlgorithms/Reduction/Reduction.hpp"

#include <math.h>
#include "RadixSort.h"

inline cl_int IntToIntFourWayPrefixSumAndShuffle_p(
	cl::Buffer &keys_i,
	cl::Buffer &vals_i,
	cl_int numElems,
	cl_int blkSize,
	cl_int bitIndx,
	cl::Buffer &blkSum_o,
	cl::Buffer &keyShuffle_o,
	cl::Buffer &valShuffle_o
) {
	cl_int closestMultiple = std::ceil(((float)numElems) / blkSize) * blkSize;

	cl_int error = 0;
	cl::CommandQueue &queue = CLFW::DefaultQueue;
	cl::Kernel &kernel = CLFW::Kernels["IntToIntFourWayPrefixSumWithShuffleKernel"];

	error |= CLFW::getBuffer(blkSum_o, "blkSum", CLFW::NextPow2(4 * closestMultiple / blkSize) * sizeof(cl_int));
	error |= CLFW::getBuffer(keyShuffle_o, "keyshuffle", CLFW::NextPow2(closestMultiple) * sizeof(cl_int));
	error |= CLFW::getBuffer(valShuffle_o, "valshuffle", CLFW::NextPow2(closestMultiple) * sizeof(cl_int));

	error |= kernel.setArg(0, keys_i);
	error |= kernel.setArg(1, vals_i);
	error |= kernel.setArg(2, bitIndx);
	error |= kernel.setArg(3, numElems);
	error |= kernel.setArg(4, cl::Local(4 * blkSize * sizeof(cl_int)));
	error |= kernel.setArg(5, cl::Local(blkSize * sizeof(cl_int)));
	error |= kernel.setArg(6, cl::Local(blkSize * sizeof(cl_int)));
	error |= kernel.setArg(7, blkSum_o);
	error |= kernel.setArg(8, keyShuffle_o);
	error |= kernel.setArg(9, valShuffle_o);

	error |= queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(closestMultiple), cl::NDRange(blkSize));
	return error;
}

inline cl_int MoveIntToIntElements_p(
	cl::Buffer &keyShuffle_i,
	cl::Buffer &valShuffle_i,
	cl_int numElems,
	cl::Buffer &blkSum_i,
	cl::Buffer &prefixBlkSum_i,
	cl_int blkSize,
	cl_int bitIndx,
	cl::Buffer &keys_o,
	cl::Buffer &vals_o)
{
	cl_int closestMultiple = std::ceil(((float)numElems) / blkSize) * blkSize;

	cl_int error = 0;
	cl::CommandQueue &queue = CLFW::DefaultQueue;
	cl::Kernel &kernel = CLFW::Kernels["MoveIntToIntElementsKernel"];

	CLFW::getBuffer(keys_o, "4waykeysresult", CLFW::NextPow2(closestMultiple) * sizeof(cl_int));
	CLFW::getBuffer(vals_o, "4wayvalsresult", CLFW::NextPow2(closestMultiple) * sizeof(cl_int));

	kernel.setArg(0, keyShuffle_i);
	kernel.setArg(1, valShuffle_i);
	kernel.setArg(2, cl::Local(4 * blkSize * sizeof(cl_int)));
	kernel.setArg(3, cl::Local(4 * blkSize * sizeof(cl_int)));
	kernel.setArg(4, blkSum_i);
	kernel.setArg(5, prefixBlkSum_i);
	kernel.setArg(6, keys_o);
	kernel.setArg(7, vals_o);
	kernel.setArg(8, bitIndx);
	kernel.setArg(9, closestMultiple);

	error |= queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(closestMultiple), cl::NDRange(blkSize));
	return error;
}

inline cl_int RadixSortIntToInt_p(
	cl::Buffer &keys_io,
	cl::Buffer &vals_io,
	cl_int numElems,
	cl_int numBits,
	std::string uniqueString)
{
	cl_int error = 0;
	cl_int blkSize = 64;
	cl_int closestMultiple = std::ceil(((float)numElems) / blkSize) * blkSize;
	cl_int numBlks = closestMultiple / blkSize;

	cl::Buffer keyShuffle, valShuffle, blkSum, prefixBlkSum;
	error |= CLFW::getBuffer(prefixBlkSum, "prefixBlkSum", CLFW::NextPow2(4 * numElems / blkSize) * sizeof(cl_int));
	for (int i = 0; i < numBits; i += 2) {
		error |= IntToIntFourWayPrefixSumAndShuffle_p(keys_io, vals_io, numElems, blkSize, i, blkSum, keyShuffle, valShuffle);
		error |= StreamScan_p(blkSum, 4 * numBlks, "", prefixBlkSum);
		error |= MoveIntToIntElements_p(keyShuffle, valShuffle, numElems, blkSum, prefixBlkSum, blkSize, i, keys_io, vals_io);
	}

	return error;
}
