#version 410
// Largly inspired by http://prideout.net/blog/?p=64

out vec4 finalColor;

/* Volume and transfer function textures */
uniform sampler3D texture0;
uniform sampler2D texture1;

/* Uniforms for ray generation */
uniform mat4 model_view;
uniform mat4 projection;
uniform float focal_length;
uniform vec2 window_size;
uniform vec3 ray_origin;
uniform bool perturbation;

uniform int samples;
const float maxDist = sqrt(2.0);
float step_size = maxDist/float(samples);
float s0s = (step_size*float(samples))/maxDist;

//uniform float Absorption = 0.0;

//int numSamples = 128;
//int maxSamples = 128;
struct Ray {
	vec3 origin;
	vec3 dir;
};

struct AABB {
	vec3 minimum;
	vec3 maximum;
};

bool IntersectBox(Ray r, AABB aabb, out float t0, out float t1) 
{
	vec3 invR = 1.0 / r.dir;
	vec3 tbot = invR * (aabb.minimum-r.origin);
    vec3 ttop = invR * (aabb.maximum-r.origin);
    vec3 tmin = min(ttop, tbot);
    vec3 tmax = max(ttop, tbot);
    vec2 t = max(tmin.xx, tmin.yz);
    t0 = max(t.x, t.y);
    t = min(tmax.xx, tmax.yz);
    t1 = min(t.x, t.y);
    return t0 <= t1;
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
	/* Create fragment ray */
	float aspect = window_size.x / window_size.y;
	vec3 ray_direction;
	ray_direction.xy = 2.0 * gl_FragCoord.xy / window_size - 1.0;
	ray_direction.x *= aspect; 
	ray_direction.z = -focal_length;
	ray_direction = (vec4(ray_direction, 0) * model_view).xyz;

	Ray eye = Ray(ray_origin, normalize(ray_direction));
	
	/* Intersect the ray with an object space box. */
	AABB aabb = AABB(vec3(-1.0), vec3(+1.0));
	float tnear, tfar;
	IntersectBox(eye, aabb, tnear, tfar);
	if (tnear < 0.0) tnear = 0.0;

	vec3 ray_start = eye.origin + eye.dir * tnear;
	vec3 ray_stop = eye.origin + eye.dir * tfar;

	// Transform from object space to texture coordinate space:
	ray_start = (ray_start + 1.0) * 0.50;
	ray_stop = (ray_stop + 1.0) * 0.50;

	if(perturbation)
	{
		float random = rand(gl_FragCoord.xy);
		step_size += step_size * random * .2;
	}

	// Perform the ray marching:
	vec3 color = vec3(0.0, 0.0, 0.0);
	float alpha = 0.0;

	vec3 pos = ray_start;
	vec3 step = normalize(ray_stop-ray_start) * step_size;
	float travel = distance(ray_stop, ray_start);
	for (int i=0; i < samples && travel > 0.0; ++i, pos += step, travel -= step_size) {
		float datavalue = texture(texture0, pos).r;
		vec4 cx = texture(texture1, vec2(datavalue, 0.0));
		


		//color.a = 1.0 - pow(1.0 - color.a, s0s);
		//color.rgb *= color.a;


		color = color + (1.0 - alpha) * cx.rgb * cx.a;
		alpha = alpha + cx.a * (1 - alpha);

		//value += color;
		if (alpha > 1.0) {
			alpha = 1.0;
			break;
		}
	}

	finalColor = vec4(color, alpha);
}
