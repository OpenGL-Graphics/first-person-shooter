#version 130

uniform vec3 color;

out vec4 color_out;

void main() {
  color_out = vec4(color, 1.0);
}
