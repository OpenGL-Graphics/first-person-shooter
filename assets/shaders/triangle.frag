#version 130

in vec3 color_vert;
in vec2 texture_coord_vert;

uniform sampler2D texture_in;

out vec4 color_out;

void main() {
  color_out = texture(texture_in, texture_coord_vert) * vec4(color_vert, 1.0);
}
