#version 130

in vec2 texture_coord_vert;

uniform sampler2D texture2d;

out vec4 color_out;

void main() {
  float alpha = texture(texture2d, texture_coord_vert).r;
  color_out = vec4(1.0, 1.0, 1.0, alpha);
}
