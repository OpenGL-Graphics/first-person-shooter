#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

// opengl tranformation matrices
uniform mat4 model;      // object coord -> world coord
uniform mat4 view;       // world coord  -> camera coord
uniform mat4 projection; // camera coord -> ndc coord

out vec3 normal_vert;

/* Downloaded from: http://mua.github.io/matcap-webgl.html */
void main() {
  gl_Position = projection * view * model * vec4(position, 1.0);

  // normal in world space
  normal_vert = vec3(model * vec4(normal, 0.0));
}
