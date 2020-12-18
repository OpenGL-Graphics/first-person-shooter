#version 130

in vec2 position_in;

void main() {
  gl_Position = vec4(position_in, 0.0, 1.0);
}
