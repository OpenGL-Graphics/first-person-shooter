#version 130

in vec3 texture_dir_vert;

uniform samplerCube texture_in;

out vec4 color_out;

void main() {
  color_out = texture(texture_in, texture_dir_vert);
}
