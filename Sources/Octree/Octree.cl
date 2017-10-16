#ifndef OpenCL
#error Octree.cl must be compiled with OpenCL
#endif

#include "OctreeDefinitions/defs.h"
#include "GLUtilities/Boxes/Boxes.h"
#include "Octree/QuadNode.h"
#include "Octree/OctNode.h"
#include "Octree/shared.h"
#include "BoundingBox/BoundingBox.h"

__kernel void CreateQuadtreeBoxesKernel(
	__global QuadNode *quadtree,
	__global Leaf *leaves,
	BoundingBox2D bb,
	__global Box *boxes
) {
	cl_int gid = get_global_id(0);
	cl_int gs = get_global_size(0);
	Leaf l = leaves[gid];
	QuadNode parent = quadtree[l.parent];
	float2 bbCenter = BB2D_center(&bb);
	cl_float leafWidth = bb.maxwidth / (1 << (parent.level + 1));
	
	float2 quadleafOrigin = getQuadLeafOrigin(quadtree, parent, l, bb.maxwidth, bbCenter, false);

	Box b;
	b.center = make_float4(quadleafOrigin.s0, quadleafOrigin.s1, 0.0f, 1.0f);
	b.scale = make_float4(leafWidth / 2.f, leafWidth/2.f, 0.0f, 1.0f);
	b.color = make_float4(0.8f, 0.8f, 0.8f, 1.0f);

	boxes[gid] = b;
}

__kernel void CreateOctreeBoxesKernel(
	__global OctNode *octree,
	__global Leaf *leaves,
	BoundingBox3D bb,
	__global Box *boxes
) {
	cl_int gid = get_global_id(0);
	cl_int gs = get_global_size(0);
	Leaf l = leaves[gid];
	OctNode parent = octree[l.parent];
	float3 bbCenter = BB3D_center(&bb);
	cl_float leafWidth = bb.maxwidth / (1 << (parent.level + 1));
	
	float3 octleafOrigin = getOctLeafOrigin(octree, parent, l, bb.maxwidth, bbCenter);

	Box b;
	b.center = make_float4(octleafOrigin.s0, octleafOrigin.s1, octleafOrigin.s2, 1.0f);
	b.scale = make_float4(leafWidth / 2.f, leafWidth/2.f, leafWidth/2.f, 1.0f);
	b.color = make_float4(0.8f, 0.8f, 0.8f, 1.0f);

	boxes[gid] = b;
}
