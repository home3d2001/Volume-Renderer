#pragma once
#include "OctreeDefinitions/defs.h"
#include "Vector/vec.h"

#ifndef  OpenCL 
#define __constant
#endif

__constant const cl_int leaf_masks[] = { 1, 2, 4, 8 };

typedef struct Leaf {
	cl_int parent;
	cl_int quadrant;
} Leaf;

typedef struct Pair {
	cl_int first;
	cl_int last;
} Pair;

typedef struct OctreeData {
	floatn fmin;
	cl_float fwidth;
	cl_int qwidth;
	cl_int size;
	cl_int maxDepth;
} OctreeData;

inline bool compareLeaf(Leaf* first, Leaf* second) {
	if (first->parent != second->parent) return false;
	if (first->quadrant != second->quadrant) return false;
	return true;
}