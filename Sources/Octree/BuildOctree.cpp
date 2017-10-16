#include "Octree/BuildOctree.h"
#include "ParallelAlgorithms/ParallelAlgorithms.h"

#ifndef  OpenCL 
#include <stdbool.h>
#include <stdio.h>
#define __local
#define __global
#define barrier(a)
#endif

void ComputeLocalSplits(
	__global cl_int* splits,
	__global BrtNode* node, 
	bool colored, 
	bool twoD,
	__global cl_int *colors, 
	const int gid) {
	BrtNode n = node[gid];
	int dim = (twoD) ? DIM2D : DIM3D;
	const int currentLenPerDim = n.lcp.len / dim;
	const int left = n.left;
	const int right = left + 1;
	if (!n.left_leaf) {
		if (!colored || colors[left] == -2)
			splits[left] = node[left].lcp.len / dim - currentLenPerDim;
		else
			splits[left] = 0;
  }
  if (!n.right_leaf) {
		if (!colored || colors[right] == -2)
			splits[right] = node[right].lcp.len / dim - currentLenPerDim;
		else
			splits[right] = 0;
  }
}

// Given a lcp, returns the i'th quadrant starting from the most local.
// Suppose node.lcp is 1010011 and DIM == 2. The
// quadrantInLcp(node, 0) returns 01 (1010011)
//                                        ^^
// quadrantInLcp(node, 1) returns 10 (1010011)
//                                      ^^
// quadrantInLcp(node, 2) returns 10 (1010011)
//                                    ^^
int get2DQuadrantFromBrt(
	BrtNode* brt_node, 
	const int i) {
  const int mask = 3;
  const int rem = brt_node->lcp.len% DIM2D;
  int rshift = i * DIM2D + rem;
  big buMask = makeBig(mask);
	big temp = shiftBigRight(&brt_node->lcp.bu, rshift);
	temp = andBig(&temp, &buMask);
	return temp.blk[0];
}

int get3DQuadrantFromBrt(
	BrtNode* brt_node,
	const int i) {
	const int mask = 7;
	const int rem = brt_node->lcp.len% DIM3D;
	int rshift = i * DIM3D + rem;
	big buMask = makeBig(mask);
	big temp = shiftBigRight(&brt_node->lcp.bu, rshift);
	temp = andBig(&temp, &buMask);
	return temp.blk[0];
}

int get2DQuadrantFromLCP(
	big lcp, 
	cl_int lcpLen, 
	cl_int i) {
  const int mask = 3;
  const int rem = lcpLen % DIM2D;
  int rshift = i * DIM2D + rem;
  big temp;
  big buMask = makeBig(mask);
  temp = shiftBigRight(&lcp, rshift);
  temp = andBig(&temp, &buMask);
	return temp.blk[0];
}

int get3DQuadrantFromLCP(
	big lcp,
	cl_int lcpLen,
	cl_int i) {
	const int mask = 7;
	const int rem = lcpLen % DIM3D;
	int rshift = i * DIM3D + rem;
	big temp;
	big buMask = makeBig(mask);
	temp = shiftBigRight(&lcp, rshift);
	temp = andBig(&temp, &buMask);
	return temp.blk[0];
}

void brt2quadtree(
	__global BrtNode* BRT, 
	const cl_int totalBrtNodes, 
	__global QuadNode* quadtree, 
  const cl_int totalQuadNodes, 
	__global cl_int* local_splits, 
	__global cl_int* prefix_sums, 
  __global cl_int* flags, 
	const cl_int gid) {
	BrtNode brtNode = BRT[gid];
  QuadNode n;

  /* Get total number of nodes to generate, and where to put them */
  const cl_int mySplits = local_splits[gid];
  const cl_int startIndx = (gid == 0) ? 0 : prefix_sums[gid - 1];

  /* Build first node in the list */
  for (int i = 0; i < mySplits; ++i) {
    /* If this is the first node, we need to find the first BRT ancestor with a split, and use 
       that BRT Node's last split as the parent.
       If this isn't the first node, the parent is just the previously generated node. */

    if (i == 0) {
			if (gid == 0) {
				n.parent = -1;
			}
			else {
				/* Find the first ancestor containing a split... */
				int parentBRTIndx = brtNode.parent;
				if (parentBRTIndx > 0)
					while (local_splits[parentBRTIndx] == 0)
						parentBRTIndx = BRT[parentBRTIndx].parent;
				n.parent = (parentBRTIndx == 0) ? local_splits[0] - 1 : prefix_sums[parentBRTIndx - 1] + local_splits[parentBRTIndx] - 1;
			}
    }
    else { n.parent = startIndx + i - 1; }

    n.level = ((brtNode.lcp.len/ DIM2D) - mySplits) + i + 1;
    n.quadrant = get2DQuadrantFromBrt(&brtNode, mySplits - 1 - i);

    /* Write all fields except child to avoid race conditions */
    quadtree[startIndx + i].level = n.level;
    quadtree[startIndx + i].parent = n.parent;
    quadtree[startIndx + i].quadrant = n.quadrant;

    /* Add this first node to its parent */
		if (n.parent != -1)
			quadtree[n.parent].children[n.quadrant] = startIndx + i;
  }
}

void brt2octree(
	__global BrtNode* BRT,
	const cl_int totalBrtNodes,
	__global OctNode* octree,
	const cl_int totalOctNodes,
	__global cl_int* local_splits,
	__global cl_int* prefix_sums,
	__global cl_int* flags,
	const cl_int gid) {
	BrtNode brtNode = BRT[gid];
	OctNode n;

	/* Get total number of nodes to generate, and where to put them */
	const cl_int mySplits = local_splits[gid];
	const cl_int startIndx = (gid == 0) ? 0 : prefix_sums[gid - 1];

	/* Build first node in the list */
	for (int i = 0; i < mySplits; ++i) {
		/* If this is the first node, we need to find the first BRT ancestor with a split, and use
		that BRT Node's last split as the parent.
		If this isn't the first node, the parent is just the previously generated node. */

		if (i == 0) {
			if (gid == 0) {
				n.parent = -1;
			}
			else {
				/* Find the first ancestor containing a split... */
				int parentBRTIndx = brtNode.parent;
				if (parentBRTIndx > 0)
					while (local_splits[parentBRTIndx] == 0)
						parentBRTIndx = BRT[parentBRTIndx].parent;
				n.parent = (parentBRTIndx == 0) ? local_splits[0] - 1 : prefix_sums[parentBRTIndx - 1] + local_splits[parentBRTIndx] - 1;
			}
		}
		else { n.parent = startIndx + i - 1; }

		n.level = ((brtNode.lcp.len / DIM3D) - mySplits) + i + 1;
		n.quadrant = get3DQuadrantFromBrt(&brtNode, mySplits - 1 - i);

		/* Write all fields except child to avoid race conditions */
		octree[startIndx + i].level = n.level;
		octree[startIndx + i].parent = n.parent;
		octree[startIndx + i].quadrant = n.quadrant;

		/* Add this first node to its parent */
		if (n.parent != -1)
			octree[n.parent].children[n.quadrant] = startIndx + i;
	}
}

void brt2quadtree_init(
	__global QuadNode* quadtree, 
	const int gid) {
  /* Start all nodes as having only leaves for children. */
  QuadNode n;
  for (int i = 0; i < (1 << DIM2D); ++i) n.children[i] = -1;
	n.leaf = 0;

  /* Initialize the root quadnode */
  if (gid == 0) {
    n.parent = n.level = n.quadrant = -1;
    n.level = 0;
  }
  quadtree[gid] = n;
}

void brt2octree_init(
	__global OctNode* octreetree,
	const int gid) {
	/* Start all nodes as having only leaves for children. */
	OctNode n;
	for (int i = 0; i < (1 << DIM3D); ++i) n.children[i] = -1;
	n.leaf = 0;
	/* Initialize the root octnode */
	if (gid == 0) {
		n.parent = n.level = n.quadrant = -1;
		n.level = 0;
	}
	octreetree[gid] = n;
}

// gid is between 0 and 4/8X the octree size.
void ComputeQuadLeaves(
	__global QuadNode *quadtree,
	__global Leaf *leaves,
	__global cl_int *leafPredicates, 
	cl_int quadtreeSize, 
	cl_int gid) {
  int parentIndex = gid / 4;
  int leafIndex = gid % 4;
  QuadNode n = quadtree[parentIndex];
	int isLeaf = n.children[leafIndex] == -1;
  Leaf L;
  if (isLeaf) {
    L.parent = parentIndex;
    L.quadrant = leafIndex;
  }
  else {
    L.parent = -1;
    L.quadrant = -1;
  }
  leaves[gid] = L;
  leafPredicates[gid] = isLeaf;
}

void ComputeOctLeaves(
	__global OctNode *octree,
	__global Leaf *leaves,
	__global cl_int *leafPredicates,
	cl_int quadtreeSize,
	cl_int gid) {
	int parentIndex = gid / 8;
	int leafIndex = gid % 8;
	OctNode n = octree[parentIndex];
	int isLeaf = n.children[leafIndex] == -1;
	Leaf L;
	if (isLeaf) {
		L.parent = parentIndex;
		L.quadrant = leafIndex;
	}
	else {
		L.parent = -1;
		L.quadrant = -1;
	}
	leaves[gid] = L;
	leafPredicates[gid] = isLeaf;
}

inline void getQuadNodeLCP(
	big* lcp, 
	cl_int *lcpLen, 
	__global QuadNode *octree, 
	int gid) {
  QuadNode octnode = octree[gid];
	*lcp = makeBig(0);
  cl_int level = 0;
  while (octnode.parent != -1) {
    big temp = makeBig(octnode.quadrant);
		temp = shiftBigLeft(&temp, DIM * level);
    *lcp = orBig(lcp, &temp);
    level++;
    octnode = octree[octnode.parent];
  }
  *lcpLen = level * DIM;
}

//Note, assumes root exists AKA octreeSize > 0
//LCP does not include leaf.
inline int searchForQuadNode(
	big lcp, 
	int lcpLength, 
	__global QuadNode *octree) {
  if (lcpLength == 0) return 0;
  QuadNode current = octree[0];
  int index = -1;
  for (int i = 0; i < lcpLength / DIM; ++i) {
    int quadrant = get2DQuadrantFromLCP(lcp, lcpLength, i);
    if ((current.leaf & quadrant) != 0) return -1; 
    index = current.children[quadrant];
    current = octree[current.children[quadrant]];
  }
  return index;
}

// Use Z-order to determine if the current node in the new quadtree already exists in
// the old one. If it's a dup, then put the current node's original address in the
// predicate buffer. Otherwise, predicate with -1, meaning delete this node.
void PredicateDuplicateQuadNodes(
	__global QuadNode *origOT, 
	__global QuadNode *newOT, 
	__global cl_int *predicates, 
	int newOTSize, 
	int gid) {
  cl_int index;
  {
    big LCP;
    cl_int lcpLen = 0;
    getQuadNodeLCP(&LCP, &lcpLen, newOT, gid);
    index = searchForQuadNode(LCP, lcpLen, origOT);
  }

  if (index == -1) {
    predicates[gid] = -1; //Do not compact me.
  }
  else {
    predicates[gid] = index;
  }

  //OctNode current = origOT[gid];
  //current.parent++;
  //origOT[gid] = current;
  //for (int i = 0; i < DIM; ++i) {
  //  //When I perish, have the original node adopt my children. ;(
  //  //newOT[gid].parent = current.level;
  //  newOT[gid] = current;
  //  barrier(CLK_GLOBAL_MEM_FENCE);
  //}


  //else {
  //  barrier(CLK_GLOBAL_MEM_FENCE)
  //  // current.children[0];
  //  //for (int i = 0; i < DIM; ++i) {
  //  //  if ((current.leaf & (1 << i)) == 0) {
  //  //    newOT[gid].parent = 1;
  //  ////    newOT[current.children[i]].parent = index;
  //  //  }
  //  //}
  //}
}


/* Octree Code */
#ifndef OpenCL
#undef __local
#undef __global
#endif
