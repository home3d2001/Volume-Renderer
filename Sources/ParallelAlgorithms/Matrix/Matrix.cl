static float4 multiplyM4V4(float16 m, float4 v) {
		float4 temp = {
			m.s0 * v.s0 + m.s1 * v.s1 + m.s2 * v.s2 + m.s3 * v.s3,
			m.s4 * v.s0 + m.s5 * v.s1 + m.s6 * v.s2 + m.s7 * v.s3,
			m.s8 * v.s0 + m.s9 * v.s1 + m.sa * v.s2 + m.sb * v.s3,
			m.sc * v.s0 + m.sd * v.s1 + m.se * v.s2 + m.sf * v.s3,
		};
		return temp;
	}

	__kernel void multiplyM4V2Kernel(
		__global float2 *v, float16 m, __global float2 *o
	) 
	{
		float2 vec = v[get_global_id(0)];
		float4 augmented = { vec.x, vec.y, 0.0, 1.0 };
		float4 augmentedResult = multiplyM4V4(m, augmented);
		float2 out = { augmentedResult.s0, augmentedResult.s1 };
		o[get_global_id(0)] = out;
	}

	__kernel void multiplyM4V4Kernel(
		__global float4 *v, float16 m, __global float4 *o
	) 
	{
		float4 vec = v[get_global_id(0)];
		vec.s3 = 1.0;  // Temporary kludge, treat all float4s as points.
		float4 result = multiplyM4V4(m, vec);
		o[get_global_id(0)] = result;
	}