#version 410
uniform sampler2D input_texture;

in vec2 ftexcoord;
out vec4 finalColor;

void main() {
  finalColor = texture(input_texture, ftexcoord * .5 + .5);
}
