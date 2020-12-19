#version 130

in vec2 position;
in vec3 color;
in vec2 texture_coord;

// opengl tranformation matrices
uniform mat4 model;      // object coord -> world coord
uniform mat4 view;       // world coord  -> camera coord
uniform mat4 projection; // camera coord -> ndc coord

out vec3 color_vert;
out vec2 texture_coord_vert;

void main() {
  gl_Position = projection * view * model * vec4(position, 0.0, 1.0);
  color_vert = color;
  texture_coord_vert = texture_coord;
}
