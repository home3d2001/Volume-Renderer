#include "CellResolution/Conflict.h"
#include "CellResolution/LineTransform.h"

__kernel void CountResolutionPointsKernel(
		__global Conflict* conflicts,
		__global intn* qPoints,
		__global ConflictInfo* info_array,
		__global int* resolutionCounts
		)
	{
		const int gid = get_global_id(0);
		//    if (gid == 0) {
		//      printf("gpu size of conflict info: %d\n", sizeof(ConflictInfo));
		//    }
		Conflict c = conflicts[gid];
		ConflictInfo info = { 0 };

		// debug
		bool debug = false;//(gid > 1 && gid < 10);
		info.line_pairs[0].s0 = debug ? 1 : 0;

		intn q1 = qPoints[c.q1[0]];
		intn q2 = qPoints[c.q1[1]];
		intn r1 = qPoints[c.q2[0]];
		intn r2 = qPoints[c.q2[1]];

		if (q1.x == q2.x && q1.y == q2.y) {
			printf("%d Degenerate line found\n", gid);
		}

		if (r1.x == r2.x && r1.y == r2.y) {
			printf("%d Degenerate line found\n", gid);
		}

		if (debug) {
			printf("gpu\n");
			//      printf("gpu: q1: (%d, %d)\n", q1.x, q1.y);
			//      printf("gpu: q2: (%d, %d)\n", q2.x, q2.y);
			//      printf("gpu: r1: (%d, %d)\n", r1.x, r1.y);
			//      printf("gpu: r2: (%d, %d)\n", r2.x, r2.y);
			//      printf("gpu: o: (%d, %d)\n", c.origin.x, c.origin.y);
			//      printf("gpu: w: %d\n", c.width);
		}
		info.padding[0] = gid;
		sample_conflict_count(&info, q1, q2, r1, r2, c.origin, c.width);
		if (debug) {
			printf("%d gpu - info.num_samples = %d\n", gid, info.num_samples);
		}

		info_array[gid] = info;
		resolutionCounts[gid] = info.num_samples;
	}

		__kernel void PredicatePointToConflictKernel(
		__global cl_int* scannedNumPtsPerConflict,
		__global cl_int* predicates
		) {
		predPntToConflict(scannedNumPtsPerConflict, predicates, get_global_id(0));
	}

	__kernel void GetResolutionPointsKernel(
		__global Conflict* conflicts_i,
		__global ConflictInfo* conflictInfo_i,
		__global cl_int* scannedNumPtsPerConflict_i,
		__global cl_int* pntToConflict_i,
		__global intn* qpoints_i,
		__global intn* resolutionPoints_o
		)
	{
		const int gid = get_global_id(0);
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