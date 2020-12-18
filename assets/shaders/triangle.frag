#version 130

uniform vec3 color_in;
out vec4 color_out;

void main() {
  color_out = vec4(color_in, 1.0);
}
