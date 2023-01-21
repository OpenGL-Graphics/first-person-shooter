#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture_coord;

// opengl tranformation matrices
uniform mat4 models[1];      // object coord -> world coord
uniform mat4 view;       // world coord  -> camera coord
uniform mat4 projection; // camera coord -> ndc coord

out vec2 texture_coord_vert;

void main() {
  gl_Position = projection * view * models[0] * vec4(position, 1.0);

  texture_coord_vert = texture_coord;
}
