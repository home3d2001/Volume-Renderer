#pragma once
#include "clfw.hpp"

#include <vector>
#include "Vector/vec.h"

//Old, not optimized.
inline cl_int AddAll(
	cl::Buffer &numbers, 
	cl_uint& gpuSum, 
	cl_int size) {
	cl_int nextPowerOfTwo = CLFW::NextPow2(size);
	if (nextPowerOfTwo != size) return CL_INVALID_ARG_SIZE;
	cl::Kernel &kernel = CLFW::Kernels["reduce"];
	cl::CommandQueue &queue = CLFW::DefaultQueue;

	//Each thread processes 2 items.
	int globalSize = size / 2;
	int suggestedLocal = kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(CLFW::DefaultDevice);
	int localSize = std::min(globalSize, suggestedLocal);

	cl::Buffer reduceResult;
	cl_int resultSize = CLFW::NextPow2(size / localSize);
	cl_int error = CLFW::getBuffer(reduceResult, "reduceResult", resultSize * sizeof(cl_uint));

	error |= kernel.setArg(0, numbers);
	error |= kernel.setArg(1, cl::Local(localSize * sizeof(cl_uint)));
	error |= kernel.setArg(2, nextPowerOfTwo);
	error |= kernel.setArg(3, reduceResult);
	error |= queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(globalSize), cl::NDRange(localSize));

	//If multiple workgroups ran, we need to do a second level reduction.
	if (suggestedLocal <= globalSize) {
		error |= kernel.setArg(0, reduceResult);
		error |= kernel.setArg(1, cl::Local(localSize * sizeof(cl_uint)));
		error |= kernel.setArg(2, resultSize);
		error |= kernel.setArg(3, reduceResult);
		error |= queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(localSize / 2), cl::NDRange(localSize / 2));
	}

	error |= queue.enqueueReadBuffer(reduceResult, CL_TRUE, 0, sizeof(cl_uint), &gpuSum);
	return error;
}

////////////////////////////////////////////////////////////////////////////////
// Compute the number of threads and blocks to use for the given reduction kernel
// For the kernels >= 3, we set threads / block to the minimum of maxThreads and
// n/2. For kernels < 3, we set to the minimum of maxThreads and n.  For kernel 
// 6, we observe the maximum specified number of blocks, because each thread in 
// that kernel can process a variable number of elements.
////////////////////////////////////////////////////////////////////////////////
inline void getReductionNumBlocksAndThreads(
	int whichKernel, 
	int n, 
	int maxBlocks, 
	int maxThreads, 
	int &blocks, 
	int &threads)
{
	if (whichKernel < 3)
	{
		threads = (n < maxThreads) ? CLFW::NextPow2(n) : maxThreads;
		blocks = (n + threads - 1) / threads;
	}
	else
	{
		threads = (n < maxThreads * 2) ? CLFW::NextPow2((n + 1) / 2) : maxThreads;
		blocks = (n + (threads * 2 - 1)) / (threads * 2);
	}

	if (whichKernel == 6)
		blocks = std::min(maxBlocks, blocks);
}

inline cl_int Reduce_s(
	std::vector<cl_int> numbers_i, 
	cl_int &result_o) {
	result_o = 0;
	for (int i = 0; i < numbers_i.size(); ++i) {
		result_o += numbers_i[i];
	}
	return CL_SUCCESS;
}

inline cl_int GeneralReduce(
	cl::Buffer inputBuffer, 
	cl_int totalNumbers, std::string uniqueString, 
	cl::Buffer &outputBuffer) 
{
	cl::CommandQueue &queue = CLFW::DefaultQueue;
	cl_int error = 0;

	// Non-Nvidia platforms can't take advantage of Nvidia SIMD warp unrolling optimizations.
	// As a result, we default to the less efficient reduction number 3.
	cl::Kernel &kernel3 = CLFW::Kernels["reduce3"];
	error |= CLFW::getBuffer(outputBuffer, uniqueString + "reduceout", CLFW::NextPow2(totalNumbers) * sizeof(cl_int));

	int maxThreads = std::min((int)kernel3.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(CLFW::DefaultDevice), 128);
	if (maxThreads == 1) {
		kernel3 = CLFW::Kernels["oneThreadReduce"];
		error |= kernel3.setArg(0, inputBuffer);
		error |= kernel3.setArg(1, outputBuffer);
		error |= kernel3.setArg(2, totalNumbers);
		error |= queue.enqueueNDRangeKernel(kernel3, cl::NullRange, cl::NDRange(1), cl::NDRange(1));
		return error;
	}
	int maxBlocks = 64;

	int whichKernel = 3;
	int numBlocks = 0, finalNumBlocks = 0;
	int numThreads = 0, finalNumThreads = 0;
	int n = totalNumbers;
	getReductionNumBlocksAndThreads(whichKernel, totalNumbers, maxBlocks, maxThreads, numBlocks, numThreads);
	int s = numBlocks;

	//Initial reduction (per block reduction)
	error |= kernel3.setArg(0, inputBuffer);
	error |= kernel3.setArg(1, outputBuffer);
	error |= kernel3.setArg(2, n);
	error |= kernel3.setArg(3, cl::Local(numThreads * sizeof(cl_int)));

	int globalWorkSize = numBlocks * numThreads;
	int localWorkSize = numThreads;

	error |= queue.enqueueNDRangeKernel(kernel3, cl::NullRange,
		cl::NDRange(globalWorkSize), cl::NDRange(localWorkSize));

	while (s > 1) {
		//Final reduction (adding each block's result together into one result)
		getReductionNumBlocksAndThreads(whichKernel, s, maxBlocks, maxThreads, finalNumBlocks, finalNumThreads);
		globalWorkSize = finalNumBlocks * finalNumThreads;
		localWorkSize = finalNumThreads;
		error |= kernel3.setArg(0, outputBuffer);
		error |= kernel3.setArg(1, outputBuffer);
		error |= kernel3.setArg(2, n);
		error |= kernel3.setArg(3, cl::Local(numThreads * sizeof(cl_int)));
		s = (s + (finalNumThreads * 2 - 1)) / (finalNumThreads * 2);
		error |= queue.enqueueNDRangeKernel(kernel3, cl::NullRange,
			cl::NDRange(globalWorkSize), cl::NDRange(localWorkSize));
	}
	return error;
}

// Specifically calibrated to the gtx 1070, which recommends 1024 threads per block.
// I'm getting about .250ms on 2^22 numbers, which is about 60GB/s
inline cl_int NvidiaReduce(
	cl::Buffer inputBuffer, 
	cl_int totalNumbers, 
	std::string uniqueString, 
	cl::Buffer &outputBuffer) {
	if (CLFW::SelectedVendor != Vendor::Nvidia) return CL_INVALID_PLATFORM;

	cl::CommandQueue &queue = CLFW::DefaultQueue;
	cl_int error = 0;

	cl::Kernel &kernel6 = CLFW::Kernels["reduce6"];
	cl::Kernel &kernel5 = CLFW::Kernels["reduce5"];

	int maxBlocks = 64;
	int maxThreads = std::min((int)kernel6.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(CLFW::DefaultDevice), 1024);

	error |= CLFW::getBuffer(outputBuffer, "resulttestnumbers", CLFW::NextPow2(totalNumbers) * sizeof(cl_int));

	int whichKernel = 6;
	int numBlocks = 0, finalNumBlocks = 0;
	int numThreads = 0, finalNumThreads = 0;
	int n = totalNumbers;
	getReductionNumBlocksAndThreads(whichKernel, totalNumbers, maxBlocks, maxThreads, numBlocks, numThreads);
	int s = numBlocks;

	//Initial reduction (per block reduction)
	error |= kernel6.setArg(0, inputBuffer);
	error |= kernel6.setArg(1, outputBuffer);
	error |= kernel6.setArg(2, n);
	error |= kernel6.setArg(3, cl::Local(numThreads * sizeof(cl_int)));

	int globalWorkSize = numBlocks * numThreads;
	int localWorkSize = numThreads;

	error |= queue.enqueueNDRangeKernel(kernel6, cl::NullRange,
		cl::NDRange(globalWorkSize), cl::NDRange(localWorkSize));

	whichKernel = 5;
	while (s > 1) {
		//Final reduction (adding each block's result together into one result)
		getReductionNumBlocksAndThreads(whichKernel, s, maxBlocks, maxThreads, finalNumBlocks, finalNumThreads);
		globalWorkSize = finalNumBlocks * finalNumThreads;
		localWorkSize = finalNumThreads;
		error |= kernel5.setArg(0, outputBuffer);
		error |= kernel5.setArg(1, outputBuffer);
		error |= kernel5.setArg(2, n);
		error |= kernel5.setArg(3, cl::Local(numThreads * sizeof(cl_int)));
		s = (s + (finalNumThreads * 2 - 1)) / (finalNumThreads * 2);
		error |= queue.enqueueNDRangeKernel(kernel5, cl::NullRange,
			cl::NDRange(globalWorkSize), cl::NDRange(localWorkSize));
	}
	return error;
}

inline cl_int Reduce_p(
	cl::Buffer numbers_i, 
	cl_int totalNumbers, 
	std::string uniqueString, 
	cl::Buffer &result_o) {
	if (CLFW::SelectedVendor != Vendor::Nvidia || totalNumbers < 64)
		return GeneralReduce(numbers_i, totalNumbers, uniqueString, result_o);
	else
		return NvidiaReduce(numbers_i, totalNumbers, uniqueString, result_o);
}

inline void MinReduceFloatn(
	std::vector<float3> &input, 
	float3& min) {
	//TODO: implement
}

inline void MaxReduceFloatn(
	std::vector<float3> &input,
	float3& max) {
	//TODO: implement
}