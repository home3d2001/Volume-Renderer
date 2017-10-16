#ifndef OpenCL
#error Boxes.cl must be compiled with OpenCL
#endif

#include "GLUtilities/Boxes/Boxes.h"

//Run 24 * the number of boxes. (one thread per point, rendered as GL_LINES)
//TODO: test this
__kernel void CreateBoxes3DVBO(
	__global Box* boxesBuffer,
	__global float *VBO
) {
	float s[3] = {1.f,1.f,1.f};
	int m[3], d[3];
	
	const int gid = get_global_id(0);
	const int bid = gid / 24;
	const int id = gid % 24;
	const int dir = id / 8;
	const int idx = id % 8;

	Box b = boxesBuffer[bid];

	m[0] = idx & (1<<0);
	m[1] = idx & (1<<1);
	m[2] = idx & (1<<2);

	d[0] = dir;
	d[1] = (dir + 1) % 3;
	d[2] = (dir + 2) % 3;

	if (m[d[0]]) s[0] *= -1;
	if (m[d[1]]) s[1] *= -1;
	if (m[d[2]]) s[2] *= -1;

	float4 point = make_float4(
		s[0] * b.scale.x + b.center.x,
		s[1] * b.scale.y + b.center.y, 
		s[2] * b.scale.z + b.center.z,
		1.0);

	const int offset = gid * 8; // each point has 8 floats.
	VBO[offset + 0] = point.x;
	VBO[offset + 1] = point.y;
	VBO[offset + 2] = point.z;
	VBO[offset + 3] = point.w;

	VBO[offset + 4] = b.color.x;
	VBO[offset + 5] = b.color.y;
	VBO[offset + 6] = b.color.z;
	VBO[offset + 7] = b.color.w;
}