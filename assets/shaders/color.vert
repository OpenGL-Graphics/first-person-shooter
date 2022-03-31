#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

// opengl tranformation matrices
uniform mat4 model;      // object coord -> world coord
uniform mat4 view;       // world coord  -> camera coord
uniform mat4 projection; // camera coord -> ndc coord

out vec3 color_vert;

void main() {
  gl_Position = projection * view * model * vec4(position, 1.0);
  color_vert = color;
}
