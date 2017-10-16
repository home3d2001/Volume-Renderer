#pragma once

/* Kernels defined in BigPredication.cl, ConflictPredication.cl, LCPPredication.cl, and Predication.cl */
#include "clfw.hpp"
#include "Predication.h"

#include <vector>

/* Integer Predication */
inline cl_int PredicateByBit_p(
	cl::Buffer &input, 
	cl_int index, 
	cl_int compared, 
	cl_int totalElements, 
	std::string uniqueString, 
	cl::Buffer &predicate) 
{
	cl_int roundSize = CLFW::NextPow2(totalElements);
	cl::CommandQueue &queue = CLFW::DefaultQueue;
	cl::Kernel &kernel = CLFW::Kernels["PredicateBitKernel"];

	cl_int error = CLFW::getBuffer(predicate, uniqueString + "predicateBit", sizeof(cl_int)* roundSize);

	error |= kernel.setArg(0, input);
	error |= kernel.setArg(1, predicate);
	error |= kernel.setArg(2, index);
	error |= kernel.setArg(3, compared);
	error |= queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(totalElements), cl::NullRange);

	return error;
}

inline cl_int PredicateByBit_s(
	std::vector<cl_int> numbers_i,
	cl_int index, 
	cl_int compared, 
	std::vector<cl_int> &predication_o)
{
	predication_o.resize(numbers_i.size());
	for (int i = 0; i < numbers_i.size(); ++i)
		BitPredicate(numbers_i.data(), predication_o.data(), index, compared, i);

	return CL_SUCCESS;
}

/* Unsigned Long Long Predication */
inline cl_int PredicateULLByBit_p(
	cl::Buffer &input, 
	cl_int index, 
	cl_int compared, 
	cl_int totalElements, 
	std::string uniqueString,
	cl::Buffer &predicate) {
	cl_int roundSize = CLFW::NextPow2(totalElements);
	cl::CommandQueue *queue = &CLFW::DefaultQueue;
	cl::Kernel *kernel = &CLFW::Kernels["PredicateULLBitKernel"];

	cl_int error = CLFW::getBuffer(predicate, uniqueString + "predicateBit", sizeof(cl_int)* roundSize);

	error |= kernel->setArg(0, input);
	error |= kernel->setArg(1, predicate);
	error |= kernel->setArg(2, index);
	error |= kernel->setArg(3, compared);
	error |= queue->enqueueNDRangeKernel(*kernel, cl::NullRange, cl::NDRange(totalElements), cl::NullRange);
	return error;
}

inline cl_int PredicateULLByBit_s(
	std::vector<unsigned long long> numbers_i,
	cl_int index, 
	cl_int compared, 
	std::vector<cl_int> &predication_o) {
	predication_o.resize(numbers_i.size());
	for (int i = 0; i < numbers_i.size(); ++i)
		BitPredicateULL(numbers_i.data(), predication_o.data(), index, compared, i);
	return CL_SUCCESS;
}
