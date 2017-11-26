#version 410
uniform sampler2D texture0;

in vec2 ftexcoord;
out vec4 finalColor;

void main() {
  finalColor = texture(texture0, ftexcoord);
}
