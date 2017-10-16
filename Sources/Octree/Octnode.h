#pragma once

// An octree node is an internal node of the octree. An octree cell
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

typedef struct OctNode {
	cl_int children[1 << DIM3D];
	cl_int leaf;
	cl_int quadrant;
	cl_int level;
	cl_int parent;
#ifdef __cplusplus
	const int& operator[](const int i) const {
		return children[i];
	}
#endif // __cplusplus
} OctNode;

static inline void init_OctNode(struct OctNode* node) {
	node->leaf = 15;
	for (int i = 0; i < (1 << DIM3D); ++i) {
		node->children[i] = -1;
	}
}

static inline void set_octree_child(struct OctNode* node, const int octant, const int child) {
	node->children[octant] = child;
	if (child > -1) {
		node->leaf &= ~leaf_masks[octant];
	}
	else {
		node->leaf |= leaf_masks[octant];
	}
}

static inline bool is_octnode_leaf(const struct OctNode* node, const int i) {
	return node->leaf & leaf_masks[i];
}

static inline void set_octree_data(struct OctNode* node, const int octant, const int data) {
	node->children[octant] = data;
}

inline bool compareOctNode(OctNode* first, OctNode* second) {
	for (int i = 0; i < 1 << DIM3D; ++i)
		if (first->children[i] != second->children[i]) return false;
	if (first->leaf != second->leaf) return false;
	return true;
}

//Optimize this
inline int getOctNode(big lcp, int lcpLength, __global OctNode *octree, __local OctNode *root) {
	big mask;
	big result;
	int numLevels = lcpLength / DIM3D;
	int shift = lcpLength % DIM3D;
	int currentIndex = 0;
	int parentIndex = 0;
	int childIndex = 0;
	OctNode node = *root;
	for (int i = 0; i < numLevels; ++i) {
		//Get child index
		int shiftAmount = (numLevels - i - 1) * DIM3D + shift;
		big mask = makeBig((DIM3D == 2) ? 3 : 7);
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

inline float3 getOctLeafOrigin(__global OctNode *octree, OctNode parent, Leaf leaf, cl_float octreeWidth, float3 bbCenter) {
	cl_int key = leaf.parent;
	OctNode node = parent;
	octreeWidth /= 2.0;
	cl_float shift = octreeWidth / (1 << (node.level));
	cl_float leafWidth = shift / 2.0;
	float3 offset = bbCenter;

	while (node.parent != -1) {
		offset.x += (node.quadrant & 1) ? shift : -shift;
		offset.y += (node.quadrant & 2) ? shift : -shift;
		offset.z += (node.quadrant & 4) ? shift : -shift;
		shift *= 2.f;
		node = octree[node.parent];
	}

	offset.x += (leaf.quadrant & 1) ? leafWidth : -leafWidth;
	offset.y += (leaf.quadrant & 2) ? leafWidth : -leafWidth;
	offset.z += (leaf.quadrant & 4) ? leafWidth : -leafWidth;

	return offset;
}

#ifndef OpenCL
#undef __constant
#undef __local
#undef __global
#endif
