#version 410
uniform sampler3D texture0;
uniform sampler2D texture1;

in vec3 ftexcoord;
out vec4 finalColor;

void main() {
	// No transfer function stuff yet.
	highp float value = texture(texture0, ftexcoord).r;
	vec4 color = texture(texture1, vec2(value, 0.0));
	finalColor = color;//vec4(value, value, value, 0.1);
}
