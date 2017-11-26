#version 410

layout(location = 0) in vec4 position;

uniform mat4 model_view;
uniform mat4 projection;

void main() {
  	gl_Position = projection * model_view * position;
}
