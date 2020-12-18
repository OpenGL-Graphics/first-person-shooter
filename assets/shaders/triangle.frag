#version 130

in vec3 color_out_vert;

out vec4 color_out;

void main() {
  color_out = vec4(color_out_vert, 1.0);
}
