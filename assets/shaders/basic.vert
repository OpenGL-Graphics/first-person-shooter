#version 330 core

layout (location = 0) in vec3 position;

// opengl tranformation matrices
uniform mat4 model;      // object coord -> world coord
uniform mat4 view;       // world coord  -> camera coord
uniform mat4 projection; // camera coord -> ndc coord

void main() {
  gl_Position = projection * view * model * vec4(position, 1.0);
}
