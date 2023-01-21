#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

#define MAX_N_INSTANCES 10

// opengl tranformation matrices
// array cannot have a dyamic size in glsl
// works if a smaller-sized array (size < max) is passed from the cpu host
uniform mat4 models[MAX_N_INSTANCES]; // object coord -> world coord
uniform mat4 view; // world coord  -> camera coord
uniform mat4 projection; // camera coord -> ndc coord

out vec3 position_vert;
out vec3 normal_vert;

void main() {
  mat4 model = models[gl_InstanceID];
  gl_Position = projection * view * model * vec4(position, 1.0);

  // fragment position is in world space (same as light)
  position_vert = (model * vec4(position, 1.0)).xyz;
  normal_vert = normal;
}
