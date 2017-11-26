#version 410

layout(location = 0) in vec4 position;

uniform float width;
uniform float height;
uniform mat4 matrix;

out vec2 ftexcoord;

void main() {
  	gl_Position = matrix * vec4(position.x, position.y, 0.0, 1.0);
  	ftexcoord = (vec2(position.x, position.y) + 1.0) * .5;
}
