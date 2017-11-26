#include "Line/Line.h"
#include "Vector/vec.h"

__kernel void GeneratePointsVBO(
	__global float3 *points,
	__global Line *lines,
	__global float *vbo
) {
	float4 colors[20] = {
		make_float4(.729f, .604f, 0.f, 1.f),
		make_float4(0, .565, .071, 1.f),
		make_float4(.729, .035, 0, 1.f),
		make_float4(.153, .043, .502, 1.f),
		make_float4(.941f,.78f, 0.f, 1.f),
		make_float4(0, .729, .009, 1.f),
		make_float4(.941, .047, 0, 1.f),
		make_float4(.20, .059, .647, 1.f),
		make_float4(1.f, .839f, .071f, 1.f),
		make_float4(.059, .831, .153, 1.f),
		make_float4(1.00, .118, .071, 1.f),
		make_float4(.282, .133, .769, 1.f),
		make_float4(1.00f, .871f, .251f, 1.f),
		make_float4(.212, .855, .29, 1.f),
		make_float4(1.00, .29, .251, 1.f),
		make_float4(.392, .267, .796, 1.f),
		make_float4(1.00f, .902f, .424f, 1.f),
		make_float4(.373, .886, .435, 1.f),
		make_float4(1.00, .451, .424, 1.f),
		make_float4(.514, .412, .843, 1.f),
	};

	int gid = get_global_id(0);
	Line line = lines[gid];
	float3 first = points[line.first];
	float3 second = points[line.second];
	float4 color = colors[line.color % 20];

	// 2 point and 2 color
	int offset = gid * 16;
	vbo[offset + 0] = first.x;
	vbo[offset + 1] = first.y;
	vbo[offset + 2] = first.z;
	vbo[offset + 3] = 1.0;

	vbo[offset + 4] = color.s0;
	vbo[offset + 5] = color.s1;
	vbo[offset + 6] = color.s2;
	vbo[offset + 7] = 1.0;

	vbo[offset + 8] = second.x;
	vbo[offset + 9] = second.y;
	vbo[offset + 10] = second.z;
	vbo[offset + 11] = 1.0;

	vbo[offset + 12] = color.s0;
	vbo[offset + 13] = color.s1;
	vbo[offset + 14] = color.s2;
	vbo[offset + 15] = 1.0;
}