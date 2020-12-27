#version 130

in vec3 position;
in vec3 color;
in vec3 texture_dir;

// opengl tranformation matrices
uniform mat4 model;      // object coord -> world coord
uniform mat4 view;       // world coord  -> camera coord
uniform mat4 projection; // camera coord -> ndc coord

out vec3 color_vert;
out vec3 texture_dir_vert;

void main() {
  gl_Position = projection * view * model * vec4(position, 1.0);
  color_vert = color;
  texture_dir_vert = texture_dir;
}
