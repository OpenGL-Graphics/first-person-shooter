#version 130

in vec3 color_vert;

out vec4 color_out;

void main() {
  color_out = vec4(color_vert, 1.0);
}
