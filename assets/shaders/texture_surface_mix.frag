#version 460 core

in vec2 texture_coord_vert;

uniform sampler2D texture2d_1;
uniform sampler2D texture2d_2;

out vec4 color_out;

void main() {
  // sample texel (pixel) from texture bound to sampler at coordinate
  vec4 color1 = texture(texture2d_1, texture_coord_vert);
  vec4 color2 = texture(texture2d_2, texture_coord_vert);

  // blend between two textures
  color_out = mix(color1, color2, 0.5);
}
