#pragma once

// A quadtree node is an internal node of the quadtree. A quadtree cell
// is a general term that refers to both internal nodes and leaves.
#include "OctreeDefinitions/defs.h"
#include "Dimension/dim.h"
#include "BigUnsigned/BigNum.h"
#include "Vector/vec.h"
#include "Octree/Shared.h"

#ifndef  OpenCL 
#define __constant
#define __local
#define __global
#endif

typedef struct QuadNode {
  cl_int children[1 << DIM2D];
  cl_int leaf;
  cl_int quadrant;
  cl_int level;
  cl_int parent;
#ifdef __cplusplus
  const int& operator[](const int i) const {
    return children[i];
  }
#endif // __cplusplus
} QuadNode;

static inline void init_QuadNode(struct QuadNode* node) {
  node->leaf = 15;
  for (int i = 0; i < (1 << DIM2D); ++i) {
    node->children[i] = -1;
  }
}

static inline void set_child(struct QuadNode* node, const int octant, const int child) {
  node->children[octant] = child;
  if (child > -1) {
    node->leaf &= ~leaf_masks[octant];
  }
  else {
    node->leaf |= leaf_masks[octant];
  }
}

static inline bool is_leaf(const struct QuadNode* node, const int i) {
  return node->leaf & leaf_masks[i];
}

static inline void set_data(struct QuadNode* node, const int octant, const int data) {
  node->children[octant] = data;
}

inline bool compareQuadNode(QuadNode* first, QuadNode* second) {
  for (int i = 0; i < 1 << DIM2D; ++i)
    if (first->children[i] != second->children[i]) return false;
  if (first->leaf != second->leaf) return false;
  return true;
}

//Optimize this
inline int getQuadNode(big lcp, int lcpLength, __global QuadNode *octree, __local QuadNode *root) {
  big mask;
  big result;
  int numLevels = lcpLength / DIM2D;
  int shift = lcpLength % DIM2D;
  int currentIndex = 0;
  int parentIndex = 0;
  int childIndex = 0;
  QuadNode node = *root;
  for (int i = 0; i < numLevels; ++i) {
    //Get child index
    int shiftAmount = (numLevels - i - 1) * DIM2D + shift;
    big mask = makeBig((DIM2D == 2) ? 3 : 7);
    mask = shiftBigLeft(&mask, shiftAmount);
    big result = andBig(&mask, &lcp);
    result = shiftBigRight(&result, shiftAmount);
    childIndex = result.blk[0];

    currentIndex = node.children[childIndex];
    if (currentIndex == -1) {
      //The current LCP sits within a leaf node.
      return parentIndex;
    }
    node = octree[currentIndex];
    parentIndex = currentIndex;
  }
  //The LCP refers to an internal node.
  return currentIndex;
}

inline float2 getQuantizedQuadLeafOrigin(__global QuadNode *octree, QuadNode parent, Leaf leaf, int octreeWidth) {
	cl_int key = leaf.parent;
	QuadNode node = parent;
	float2 offset = make_float2(0.0, 0.0);
	cl_int shift = octreeWidth / (1 << (node.level));
	cl_int leafWidth = shift / 2.0;

	while (node.parent != -1) {
		offset.x += (node.quadrant & 1) ? shift : 0;
		offset.y += (node.quadrant & 2) ? shift : 0;
		shift <<= 1;
		node = octree[node.parent];
	}

	offset.x += (leaf.quadrant & 1) ? leafWidth : 0;
	offset.y += (leaf.quadrant & 2) ? leafWidth : 0;

	return offset;
}

inline float2 getQuadLeafOrigin(__global QuadNode *octree, QuadNode parent, Leaf leaf, cl_float octreeWidth, float2 bbCenter, bool debug) {
	cl_int key = leaf.parent;
	QuadNode node = parent;
	octreeWidth /= 2.0;
	cl_float shift = octreeWidth / (1 << (node.level));
	cl_float leafWidth = shift / 2.0;
	float2 offset = bbCenter;

	while (node.parent != -1) {
		offset.x += (node.quadrant & 1) ? shift : -shift;
		offset.y += (node.quadrant & 2) ? shift : -shift;
		shift *= 2.f;
		node = octree[node.parent];
	}

	offset.x += (leaf.quadrant & 1) ? leafWidth : -leafWidth;
	offset.y += (leaf.quadrant & 2) ? leafWidth : -leafWidth;

	return offset;
}

#ifndef OpenCL
#undef __constant
#undef __local
#undef __global
#endif
