/* Kernels defined in ZOrder.cl */

#pragma once
#include "clfw.hpp"
#include "BigUnsigned/BigNum.h"
#include "z_order.h"

inline cl_int QPoints2DToZPoints_p(
	cl::Buffer &qpoints,
	cl_int totalPoints,
	cl_int reslnBits,
	std::string str,
	cl::Buffer &zpoints
)
{
	cl_int error = 0;
	cl_int globalSize = CLFW::NextPow2(totalPoints);
	bool old;
	error |= CLFW::getBuffer(zpoints, str + "zpts", globalSize * sizeof(big), old, false, CL_MEM_READ_ONLY);
	cl::Kernel kernel = CLFW::Kernels["Points2DToMortonKernel"];
	error |= kernel.setArg(0, zpoints);
	error |= kernel.setArg(1, qpoints);
	error |= kernel.setArg(2, totalPoints);
	error |= kernel.setArg(3, reslnBits);
	error |= CLFW::DefaultQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(globalSize), cl::NullRange);
	assert_cl_error(error);
	return error;
};

inline cl_int QPoints2DToZPoints_s(
	std::vector<int2> qpoints,
	cl_int reslnBits,
	std::vector<big> &zpoints
) {
	for (int gid = 0; gid < qpoints.size(); ++gid) {
		big tempBU;
		int2 tempPoint = qpoints[gid];
		xyz2z2D(&tempBU, tempPoint, reslnBits);
		zpoints[gid] = tempBU;
	}
	return CL_SUCCESS;
}

inline cl_int QPoints3DToZPoints_p(
	cl::Buffer &qpoints,
	cl_int totalPoints,
	cl_int reslnBits,
	std::string str,
	cl::Buffer &zpoints
)
{
	cl_int error = 0;
	cl_int globalSize = CLFW::NextPow2(totalPoints);
	bool old;
	error |= CLFW::getBuffer(zpoints, str + "zpts", globalSize * sizeof(big), old, false, CL_MEM_READ_ONLY);
	cl::Kernel kernel = CLFW::Kernels["Points3DToMortonKernel"];
	error |= kernel.setArg(0, zpoints);
	error |= kernel.setArg(1, qpoints);
	error |= kernel.setArg(2, totalPoints);
	error |= kernel.setArg(3, reslnBits);
	error |= CLFW::DefaultQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(globalSize), cl::NullRange);
	assert_cl_error(error);
	return error;
};