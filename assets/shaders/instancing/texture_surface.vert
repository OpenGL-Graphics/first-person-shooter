#version 460 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coord;

#define MAX_N_INSTANCES 10

// opengl tranformation matrices
uniform mat4 models[MAX_N_INSTANCES];      // object coord -> world coord
uniform mat4 view;       // world coord  -> camera coord
uniform mat4 projection; // camera coord -> ndc coord

out vec2 texture_coord_vert;

void main() {
  mat4 model = models[gl_InstanceID];
  gl_Position = projection * view * model * vec4(position, 0.0, 1.0);

  texture_coord_vert = texture_coord;
}
