/* Kernels defined in Quantize.cl */

#pragma once
#include "Quantize.h"
#include "BoundingBox/BoundingBox.h"
#include "clfw.hpp"

inline cl_int Quantize2DPoints_p(
	cl::Buffer &points_i,
	cl_uint numPoints,
	const BoundingBox2D bb,
	const int reslnWidth,
	std::string str,
	cl::Buffer &qPoints_o
	) {
	cl_int error = 0;
	cl_int roundSize = CLFW::NextPow2(numPoints);
	error |= CLFW::getBuffer(qPoints_o, str + "qpts", sizeof(int2)*roundSize);
	cl::Kernel kernel = CLFW::Kernels["Quantize2DPointsKernel"];
	error |= kernel.setArg(0, points_i);
	error |= kernel.setArg(1, qPoints_o);
	error |= kernel.setArg(2, bb.minimum);
	error |= kernel.setArg(3, bb.maxwidth);
	error |= kernel.setArg(4, reslnWidth);
	error |= CLFW::DefaultQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(numPoints), cl::NullRange);
	assert_cl_error(error);
	return error;
}

inline cl_int Quantize2DPoints_s(
	std::vector<float2> &points_i,
	const BoundingBox2D bb,
	const int reslnWidth,
	std::vector<int2> &qPoints_o
) {
	qPoints_o.resize(points_i.size());
	for (int i = 0; i < points_i.size(); ++i) {
		qPoints_o[i] = Quantize2DPoint(&points_i[i], &bb.minimum, reslnWidth, bb.maxwidth);
	}
	return CL_SUCCESS;
}

inline cl_int Quantize3DPoints_p(
	cl::Buffer &points_i,
	cl_uint numPoints,
	const BoundingBox3D bb,
	const int reslnWidth,
	std::string str,
	cl::Buffer &qPoints_o
) {
	cl_int error = 0;
	cl_int roundSize = CLFW::NextPow2(numPoints);
	error |= CLFW::getBuffer(qPoints_o, str + "qpts", sizeof(int3)*roundSize);
	cl::Kernel kernel = CLFW::Kernels["Quantize3DPointsKernel"];
	error |= kernel.setArg(0, points_i);
	error |= kernel.setArg(1, qPoints_o);
	error |= kernel.setArg(2, bb.minimum);
	error |= kernel.setArg(3, bb.maxwidth);
	error |= kernel.setArg(4, reslnWidth);
	error |= CLFW::DefaultQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(numPoints), cl::NullRange);
	assert_cl_error(error);
	return error;
}

inline cl_int Unquantize2DPoints_p(
	cl::Buffer &qPoints_i,
	cl_uint numPoints,
	const BoundingBox2D bb,
	const int reslnWidth,
	std::string str,
	cl::Buffer &points_o
) {
	cl_int error = 0;
	cl_int roundSize = CLFW::NextPow2(numPoints);
	error |= CLFW::getBuffer(points_o, str + "pts", sizeof(float2)*roundSize);
	cl::Kernel kernel = CLFW::Kernels["Unquantize2DPointsKernel"];
	error |= kernel.setArg(0, qPoints_i);
	error |= kernel.setArg(1, points_o);
	error |= kernel.setArg(2, bb.minimum);
	error |= kernel.setArg(3, bb.maxwidth);
	error |= kernel.setArg(4, reslnWidth);
	error |= CLFW::DefaultQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(numPoints), cl::NullRange);
	assert_cl_error(error);
	return error;
}

inline cl_int Unquantize3DPoints_p(
	cl::Buffer &qPoints_i,
	cl_uint numPoints,
	const BoundingBox3D bb,
	const int reslnWidth,
	std::string str,
	cl::Buffer &points_o
) {
	cl_int error = 0;
	cl_int roundSize = CLFW::NextPow2(numPoints);
	error |= CLFW::getBuffer(points_o, str + "pts", sizeof(float3)*roundSize);
	cl::Kernel kernel = CLFW::Kernels["Unquantize3DPointsKernel"];
	error |= kernel.setArg(0, qPoints_i);
	error |= kernel.setArg(1, points_o);
	error |= kernel.setArg(2, bb.minimum);
	error |= kernel.setArg(3, bb.maxwidth);
	error |= kernel.setArg(4, reslnWidth);
	error |= CLFW::DefaultQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(numPoints), cl::NullRange);
	assert_cl_error(error);
	return error;
}

