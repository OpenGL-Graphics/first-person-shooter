#version 130

in vec2 position_in;
in vec3 color_in;

out vec3 color_out_vert;

void main() {
  gl_Position = vec4(position_in, 0.0, 1.0);
  color_out_vert = color_in;
}
