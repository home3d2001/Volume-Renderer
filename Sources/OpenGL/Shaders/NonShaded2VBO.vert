#version 410

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

uniform mat4 matrix;

out vec4 fColor;

void main() {
  gl_Position = matrix * position;
  fColor = color;
}
