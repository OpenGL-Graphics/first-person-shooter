#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texture_coord;

// opengl tranformation matrices
uniform mat4 model;      // object coord -> world coord
uniform mat4 view;       // world coord  -> camera coord
uniform mat4 projection; // camera coord -> ndc coord

out vec2 texture_coord_vert;
out vec3 position_vert;

/* modified from `assets/texture_surface.vert` */
void main() {
  gl_Position = projection * view * model * vec4(position, 0.0, 1.0);

  texture_coord_vert = texture_coord;
  position_vert = (model * vec4(position, 0.0, 1.0)).xyz;
}
