#pragma once

#include "clfw.hpp"
#include <chrono>

inline cl_int StreamScan_p(
	cl::Buffer &input_i, 
	const cl_int totalElements,
	std::string uniqueString, 
	cl::Buffer &result_i) {
	int globalSize = CLFW::NextPow2(totalElements);
	cl_int error = 0;
	bool isOld;
	cl::Kernel &kernel = CLFW::Kernels["StreamScanKernel"];
	cl::CommandQueue &queue = CLFW::DefaultQueue;

	/* Determine the number of groups required. */
	int wgSize = std::min((int)kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(CLFW::DefaultDevice), globalSize);
	int localSize = std::min(wgSize, globalSize);
	int numGroups = (globalSize / wgSize); //+1

																				 /* Each workgroup gets a spot in the intermediate buffer. */
	cl::Buffer intermediate, intermediateCopy;
	error |= CLFW::getBuffer(intermediate, uniqueString + "I", sizeof(cl_int) * numGroups);
	error |= CLFW::getBuffer(intermediateCopy, uniqueString + "Icopy",
		sizeof(cl_int) * numGroups, isOld);
	if (!isOld)
		error |= queue.enqueueFillBuffer<cl_int>(
			intermediateCopy, { -1 }, 0, sizeof(cl_int) * numGroups);
	error |= queue.enqueueCopyBuffer(intermediateCopy, intermediate, 0, 0,
		sizeof(cl_int) * numGroups);

	/* Call the kernel */
	error |= kernel.setArg(0, input_i);
	error |= kernel.setArg(1, result_i);
	error |= kernel.setArg(2, intermediate);
	error |= kernel.setArg(3, cl::Local((localSize + 1) * sizeof(cl_int)));
	error |= kernel.setArg(4, totalElements);
	error |= queue.enqueueNDRangeKernel(
		kernel, cl::NullRange, cl::NDRange(globalSize), cl::NDRange(localSize));
	return error;
}

inline cl_int StreamScan_s(
	std::vector<cl_int> &buffer, 
	std::vector<cl_int> &result) {
	cl_int size = buffer.size();
	cl_int nextPowerOfTwo = (int)pow(2, ceil(log(size) / log(2)));
	cl_int intermediate = -1;
	cl_int* localBuffer;
	cl_int* scratch;
	cl_int sum = 0;

	localBuffer = (cl_int*)std::malloc(sizeof(cl_int)* nextPowerOfTwo);
	scratch = (cl_int*)std::malloc(sizeof(cl_int)* nextPowerOfTwo);

	//INIT
	for (cl_int i = 0; i < size; i++)
		localBuffer[i] = scratch[i] = buffer[i];
	for (cl_int i = size; i < nextPowerOfTwo; ++i)
		localBuffer[i] = scratch[i] = 0;

	//Add not necessary with only one workgroup.
	//Adjacent sync not necessary with only one workgroup.

	//SCAN
	for (cl_int i = 1; i < nextPowerOfTwo; i <<= 1) {
		for (cl_int j = 0; j < nextPowerOfTwo; ++j) {
			if (j >(i - 1))
				scratch[j] = localBuffer[j] + localBuffer[j - i];
			else
				scratch[j] = localBuffer[j];
		}
		cl_int *tmp = scratch;
		scratch = localBuffer;
		localBuffer = tmp;
	}
	for (cl_int i = 0; i < size; ++i) {
		result[i] = localBuffer[i];
	}
	std::free(localBuffer);
	std::free(scratch);

	return CL_SUCCESS;
}

inline cl_int StreamScanTest() {
	cl_int error = 0;
	cl::Kernel &kernel = CLFW::Kernels["NvidiaStreamScanKernel"];
	cl::CommandQueue *queue = &CLFW::DefaultQueue;

	int size = 1 << 22; //will be a parameter

	cl::Buffer inputBuffer, outputBuffer;
	std::vector<cl_int> input(size);
	std::vector<cl_int> output(size);
	for (int i = 0; i < size; ++i) {
		input[i] = 1;
	}

	error |= CLFW::getBuffer(inputBuffer, "scanIn", CLFW::NextPow2(size) * sizeof(cl_int));
	error |= CLFW::getBuffer(outputBuffer, "scanout", CLFW::NextPow2(size) * sizeof(cl_int));
	error |= CLFW::Upload<cl_int>(input, inputBuffer);
	CLFW::DefaultQueue.finish();

	const int wgSize = (int)kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(CLFW::DefaultDevice);
	const int globalSize = size;
	const int localSize = std::min(wgSize, globalSize);
	int currentNumWorkgroups = (globalSize / localSize) + 1;

	bool isOld;
	cl::Buffer intermediate, intermediateCopy;
	error |= CLFW::getBuffer(intermediate, "StreamScanTestIntermediate",
		sizeof(cl_int) * currentNumWorkgroups);
	error |= CLFW::getBuffer(intermediateCopy, "StreamScanTestIntermediateCopy",
		sizeof(cl_int) * currentNumWorkgroups, isOld);

	if (!isOld) {
		error |= queue->enqueueFillBuffer<cl_int>(
			intermediateCopy, { -1 }, 0, sizeof(cl_int) * currentNumWorkgroups);
		assert_cl_error(error);
	}
	error |= queue->enqueueCopyBuffer(
		intermediateCopy, intermediate, 0, 0,
		sizeof(cl_int) * currentNumWorkgroups);

	queue->finish();
	error |= kernel.setArg(0, inputBuffer);
	error |= kernel.setArg(1, outputBuffer);
	error |= kernel.setArg(2, intermediate);
	error |= kernel.setArg(3, cl::Local(localSize * sizeof(cl_int)));
	error |= kernel.setArg(4, cl::Local(localSize * sizeof(cl_int)));
	error |= queue->enqueueNDRangeKernel(
		kernel, cl::NullRange, cl::NDRange(CLFW::NextPow2(globalSize)), cl::NDRange(localSize));

	auto start = std::chrono::steady_clock::now();
	CLFW::DefaultQueue.finish();
	auto end = std::chrono::steady_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::cout << "streamscan time:" << elapsed.count() << " microseconds." << std::endl;
	
	std::vector<cl_int> cpuOutput(size);

	cpuOutput[0] = input[0];
	for (int i = 1; i < size; ++i) {
		cpuOutput[i] = cpuOutput[i - 1] + input[i];
	}

	CLFW::Download<cl_int>(outputBuffer, size, output);
	for (int i = 0; i < size; ++i) {
		assert(cpuOutput[i] == output[i]);
	}

	//needs testing
	return error;
}