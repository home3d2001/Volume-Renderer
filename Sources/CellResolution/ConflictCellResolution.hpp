#pragma once

#include "Conflict.h"

inline cl_int GetResolutionPointsInfo_p(
	cl::Buffer &conflicts_i,
	cl_int numConflicts,
	cl::Buffer &qpoints_i,
	cl::Buffer &conflictInfo_o,
	cl::Buffer &numPtsPerConflict_o)
{
	cl_int error = 0;
	cl::CommandQueue &queue = CLFW::DefaultQueue;
	cl::Kernel &kernel = CLFW::Kernels["CountResolutionPointsKernel"];

	int globalSize = CLFW::NextPow2(numConflicts);
	//    cout<<"cpu sizeof conflict info: "<<sizeof(ConflictInfo)<<endl;
	error |= CLFW::getBuffer(conflictInfo_o, "conflictInfoBuffer", globalSize * sizeof(ConflictInfo));
	error |= CLFW::getBuffer(numPtsPerConflict_o, "numPtsPerConflict", globalSize * sizeof(cl_int));

	error |= kernel.setArg(0, conflicts_i);
	error |= kernel.setArg(1, qpoints_i);
	error |= kernel.setArg(2, conflictInfo_o);
	error |= kernel.setArg(3, numPtsPerConflict_o);
	error |= queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(numConflicts), cl::NullRange);
	return error;
}

inline cl_int GetResolutionPointsInfo_s(
	vector<Conflict> &conflicts_i,
	vector<intn> &qpoints_i,
	vector<ConflictInfo> &conflictInfo_o,
	vector<cl_int> &numPtsPerConflict_o) {
	conflictInfo_o.resize(conflicts_i.size());
	numPtsPerConflict_o.resize(conflicts_i.size());
	for (int gid = 0; gid < conflicts_i.size(); ++gid) {
		Conflict c = conflicts_i[gid];
		ConflictInfo info = { 0 };

		bool debug = false;//(gid > 1 && gid < 10);
		info.line_pairs[0].s0 = debug ? 1 : 0;

		intn q1 = qpoints_i[c.q1[0]];
		intn q2 = qpoints_i[c.q1[1]];
		intn r1 = qpoints_i[c.q2[0]];
		intn r2 = qpoints_i[c.q2[1]];
		if (debug) {
			printf("cpu\n");
			//        printf("cpu: q1: (%d, %d)\n", q1.x, q1.y);
			//        printf("cpu: q2: (%d, %d)\n", q2.x, q2.y);
			//        printf("cpu: r1: (%d, %d)\n", r1.x, r1.y);
			//        printf("cpu: r2: (%d, %d)\n", r2.x, r2.y);
			//        printf("cpu: o: (%d, %d)\n", c.origin.x, c.origin.y);
			//        printf("cpu: w: %d\n", c.width);
		}

		info.padding[0] = gid;
		sample_conflict_count(&info, q1, q2, r1, r2, c.origin, c.width);
		if (debug) {
			printf("%d cpu - info.num_samples = %d\n", gid, info.num_samples);
		}

		conflictInfo_o[gid] = info;
		numPtsPerConflict_o[gid] = info.num_samples;
	}

	return CL_SUCCESS;
}

inline cl_int PredicatePointToConflict_p(cl::Buffer &scannedNumPtsPerConflict_i, cl_int numConflicts, cl_int numResPts, cl::Buffer &predication_o) {
	cl_int error = 0;
	cl::CommandQueue &queue = CLFW::DefaultQueue;
	cl::Kernel &kernel = CLFW::Kernels["PredicatePointToConflictKernel"];

	bool isOld;
	cl::Buffer zeroBuffer;
	error |= CLFW::getBuffer(zeroBuffer, "pPntToConfZero", sizeof(cl_int) * CLFW::NextPow2(numResPts), isOld);
	error |= CLFW::getBuffer(predication_o, "pPntToConfl", sizeof(cl_int) * CLFW::NextPow2(numResPts));
	if (!isOld) error |= queue.enqueueFillBuffer<cl_int>(zeroBuffer, { 0 }, 0, sizeof(cl_int) * CLFW::NextPow2(numResPts));
	error |= queue.enqueueCopyBuffer(zeroBuffer, predication_o, 0, 0, sizeof(cl_int) * CLFW::NextPow2(numResPts));
	error |= kernel.setArg(0, scannedNumPtsPerConflict_i);
	error |= kernel.setArg(1, predication_o);
	error |= queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(numConflicts), cl::NullRange); //used to be numConflicts -1... not sure why

	return error;
}

inline cl_int PredicatePointToConflict_s(vector<cl_int> scannedNumPtsPerConflict_i, cl_int numResPts, vector<cl_int> &predication_o) {
	predication_o.resize(numResPts, 0);
	for (int i = 0; i < scannedNumPtsPerConflict_i.size(); ++i) {//used to be numConflicts -1... not sure why
		predPntToConflict(scannedNumPtsPerConflict_i.data(), predication_o.data(), i);
	}
	return CL_SUCCESS;
}

inline cl_int GetResolutionPoints_p(
	cl::Buffer &conflicts_i,
	cl::Buffer &conflictInfo_i,
	cl::Buffer &scannedNumPtsPerConflict_i,
	cl_int numResPts,
	cl::Buffer &pntToConflict_i,
	cl::Buffer &qpoints_i,
	cl::Buffer &resolutionPoints_o)
{
	cl::CommandQueue &queue = CLFW::DefaultQueue;
	cl::Kernel &kernel = CLFW::Kernels["GetResolutionPointsKernel"];
	cl_int error = 0;

	error |= CLFW::getBuffer(resolutionPoints_o, "ResPts", CLFW::NextPow2(numResPts) * sizeof(intn));
	error |= CLFW::DefaultQueue.finish();
	assert_cl_error(error);

	error |= kernel.setArg(0, conflicts_i);
	error |= kernel.setArg(1, conflictInfo_i);
	error |= kernel.setArg(2, scannedNumPtsPerConflict_i);
	error |= kernel.setArg(3, pntToConflict_i);
	error |= kernel.setArg(4, qpoints_i);
	error |= kernel.setArg(5, resolutionPoints_o);
	error |= queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(numResPts), cl::NullRange);
	return error;
}

inline cl_int GetResolutionPoints_s(
	vector<Conflict> &conflicts_i,
	vector<ConflictInfo> &conflictInfo_i,
	vector<cl_int> &scannedNumPtsPerConflict_i,
	vector<cl_int> &pntToConflict_i,
	cl_int numResPts,
	vector<intn> &qpoints_i,
	vector<intn> &resolutionPoints_o
) {
	resolutionPoints_o.resize(numResPts);
	for (int gid = 0; gid < numResPts; gid++) {
		cl_int pntToConflict = pntToConflict_i[gid];
		Conflict c = conflicts_i[pntToConflict];
		ConflictInfo info = conflictInfo_i[pntToConflict];

		intn q1 = qpoints_i[c.q1[0]];
		intn q2 = qpoints_i[c.q1[1]];
		intn r1 = qpoints_i[c.q2[0]];
		intn r2 = qpoints_i[c.q2[1]];
		cl_int totalPrevPts = (pntToConflict == 0) ? 0 : scannedNumPtsPerConflict_i[pntToConflict - 1];
		cl_int localIndx = gid - totalPrevPts;
		floatn sample;
		sample_conflict_kernel(localIndx, &info, &sample);
		resolutionPoints_o[gid] = convert_intn(sample);
	}
	return CL_SUCCESS;
}