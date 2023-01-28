#version 460 core

in vec2 texture_coord_vert;

uniform sampler2D texture2d;

out vec4 color_out;

void main() {
  vec4 color = texture(texture2d, texture_coord_vert);

  // manage transparency
  if (color.a == 0.0)
    discard;

  color_out = color;
}
