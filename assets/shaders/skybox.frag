#version 460 core

in vec3 texture_coord_vert;

uniform samplerCube texture3d;

out vec4 color_out;

void main() {
  color_out = texture(texture3d, texture_coord_vert);
}
