#version 130

in vec3 color_vert;
in vec3 texture_dir_vert;

uniform samplerCube texture_in;

out vec4 color_out;

void main() {
  color_out = texture(texture_in, texture_dir_vert) * vec4(color_vert, 1.0);
}
