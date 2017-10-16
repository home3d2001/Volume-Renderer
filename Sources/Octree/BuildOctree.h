#pragma once

#include "Octree/Octnode.h"
#include "Octree/Quadnode.h"

#ifdef __cplusplus
extern "C" {
#endif
  #include "BinaryRadixTree/BuildBRT.h"
  #include "BinaryRadixTree/BrtNode.h"
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
  #define __local
  #define __global
#endif

int get2DQuadrantFromBrt(BrtNode* brt_node, const int i);
int get2DQuadrantFromLCP(big lcp, cl_int lcpLen, cl_int i);
int get3DQuadrantFromBrt(BrtNode* brt_node, const int i);
int get3DQuadrantFromLCP(big lcp, cl_int lcpLen, cl_int i);
void ComputeLocalSplits(__global cl_int* local_splits, __global BrtNode* I, bool colored, bool twoD, __global cl_int *colors, const int gid );
void brt2quadtree_init(__global QuadNode* octree, const int gid);
void brt2octree_init( __global OctNode* octree, const int gid);
void brt2quadtree(__global BrtNode* I, const cl_int totalBrtNodes, __global QuadNode* quadtree, const cl_int totalQuadNodes, __global cl_int* local_splits, __global cl_int* prefix_sums, __global cl_int* flags, const cl_int gid);
void brt2octree( __global BrtNode* I, const cl_int totalBrtNodes, __global OctNode* octree, const cl_int totalOctNodes, __global cl_int* local_splits, __global cl_int* prefix_sums, __global cl_int* flags, const cl_int gid);
void ComputeQuadLeaves(__global QuadNode *quadtree, __global Leaf *leaves, __global cl_int *leafPredicates, cl_int quadtreeSize, cl_int gid);
void ComputeOctLeaves(__global OctNode *octree, __global Leaf *leaves, __global cl_int *leafPredicates, cl_int octreeSize, cl_int gid);
void LeafDoubleCompact(__global Leaf *inputBuffer, __global Leaf *resultBuffer, __global cl_int *lPredicateBuffer, __global cl_int *leftBuffer, const cl_int size, const int gid);
void PredicateDuplicateQuadNodes(__global QuadNode *origOT, __global QuadNode *newOT, __global cl_int *predicates, int newOTSize, int gid);

#ifndef OpenCL
  #undef __local
  #undef __global
#endif
