#version 410

layout(location = 0) in vec4 position;

uniform vec3 u_offset;
uniform float width;
uniform float height;
uniform mat4 matrix;

out vec2 ftexcoord;

void main() {
	vec3 poswithoffset = vec3(position.x * width * .5, position.y * height * .5, 0.0) + u_offset;
  	gl_Position = matrix * vec4(poswithoffset, 1.0);
  	ftexcoord = vec2(position.x, position.y);
}
