#version 410

layout(location = 0) in vec4 position;

uniform mat4 matrix;
uniform vec4 color;

out vec4 fColor;

void main() {
  gl_Position = matrix * position;
  fColor = color;
}