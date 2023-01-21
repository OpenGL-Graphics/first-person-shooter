#version 330 core

layout (location = 0) in vec3 position;

// opengl tranformation matrices
uniform mat4 models[1];      // object coord -> world coord
uniform mat4 view;       // world coord  -> camera coord
uniform mat4 projection; // camera coord -> ndc coord

out vec3 texture_coord_vert;

void main() {
  gl_Position = projection * view * models[0] * vec4(position, 1.0);

  texture_coord_vert = position;
}
