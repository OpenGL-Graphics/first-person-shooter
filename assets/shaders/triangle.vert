#version 130

in vec2 position;
in vec3 color;
in vec2 texture_coord;

out vec3 color_vert;
out vec2 texture_coord_vert;

void main() {
  gl_Position = vec4(position, 0.0, 1.0);
  color_vert = color;
  texture_coord_vert = texture_coord;
}
