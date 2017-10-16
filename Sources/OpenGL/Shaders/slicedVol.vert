#version 410

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

uniform mat4 matrix;

out vec3 ftexcoord;

void main() {
  	gl_Position = matrix * position;
  	ftexcoord = vec3(position.x + .5, position.y + .5, position.z + .5) * 1.0;
}
