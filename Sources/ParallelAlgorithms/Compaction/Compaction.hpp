#pragma once

/* Kernels defined in BigCompaction.cl, Compaction.cl, and LeafCompaction.cl*/
#include "clfw.hpp"
#include "Compaction.h"

#include <vector>
#include <math.h>

/* Integer Compaction*/
inline cl_int Compact_p(
	cl::Buffer &input_i, 
	cl::Buffer &predicate_i, 
	cl::Buffer &address_i, 
	cl_int totalElements, 
	cl::Buffer &result_i) {
	cl_int error = 0;
	bool isOld;
	cl::CommandQueue *queue = &CLFW::DefaultQueue;
	cl::Kernel *kernel = &CLFW::Kernels["CompactKernel"];
	cl::Buffer zeroBUBuffer;

	error |= kernel->setArg(0, input_i);
	error |= kernel->setArg(1, result_i);
	error |= kernel->setArg(2, predicate_i);
	error |= kernel->setArg(3, address_i);
	error |= kernel->setArg(4, totalElements);
	error |= queue->enqueueNDRangeKernel(*kernel, cl::NullRange, cl::NDRange(totalElements), cl::NullRange);
	return error;
}

inline cl_int Compact_s(
	std::vector<cl_int> &numbers_i, 
	std::vector<cl_int> &predication,
	std::vector<cl_int> &addresses,
	std::vector<cl_int> &result_o) {
	cl_int size = numbers_i.size();
	result_o.resize(size);
	for (int i = 0; i < size; ++i) {
		Compact(numbers_i.data(), result_o.data(), predication.data(), addresses.data(), size, i);
	}
	return CL_SUCCESS;
}

/* Unsigned Long Long Compaction */
inline cl_int CompactULL_p(
	cl::Buffer &input_i, 
	cl::Buffer &predicate_i, 
	cl::Buffer &address_i, 
	cl_int totalElements, 
	cl::Buffer &result_i) {
	cl_int error = 0;
	bool isOld;
	cl::CommandQueue *queue = &CLFW::DefaultQueue;
	cl::Kernel *kernel = &CLFW::Kernels["CompactULLKernel"];

	error |= kernel->setArg(0, input_i);
	error |= kernel->setArg(1, result_i);
	error |= kernel->setArg(2, predicate_i);
	error |= kernel->setArg(3, address_i);
	error |= kernel->setArg(4, totalElements);
	error |= queue->enqueueNDRangeKernel(*kernel, cl::NullRange, cl::NDRange(totalElements), cl::NullRange);
	return error;
}

inline cl_int CompactULL_s(
	std::vector<unsigned long long> &numbers_i, 
	std::vector<cl_int> &predication,
	std::vector<cl_int> &addresses,
	std::vector<unsigned long long> &result_o) 
{
	cl_int size = numbers_i.size();
	result_o.resize(size);
	for (int i = 0; i < size; ++i) {
		CompactULL(numbers_i.data(), result_o.data(), predication.data(), addresses.data(), size, i);
	}
	return CL_SUCCESS;
}
