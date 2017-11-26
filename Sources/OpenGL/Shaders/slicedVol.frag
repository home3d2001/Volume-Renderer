#version 410
uniform sampler3D texture0;
uniform sampler2D texture1;

in vec3 ftexcoord;
out vec4 finalColor;

uniform int samples;

const float maxDist = sqrt(2.0);
float step_size = maxDist/float(samples);
float s0s = (step_size*float(samples))/maxDist;

void main() {

	// No transfer function stuff yet.
	highp float value = texture(texture0, ftexcoord).r;
	vec4 color = texture(texture1, vec2(value, 0.0));

	//color.a = 1.0 - pow(1.0 - color.a, s0s);
	//color.rgb *= color.a;

	finalColor = color;
}
