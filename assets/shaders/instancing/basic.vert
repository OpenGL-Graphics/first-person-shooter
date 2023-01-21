#version 330 core

layout (location = 0) in vec3 position;

#define MAX_N_INSTANCES 10

// opengl tranformation matrices
uniform mat4 models[MAX_N_INSTANCES]; // object coord -> world coord
uniform mat4 view; // world coord  -> camera coord
uniform mat4 projection; // camera coord -> ndc coord
                         //
uniform vec3 colors[MAX_N_INSTANCES];

out vec3 color_vert;

void main() {
  mat4 model = models[gl_InstanceID];
  gl_Position = projection * view * model * vec4(position, 1.0);

  // pass instance color bcos gl_InstanceID doesn't exist in fragment shader
  color_vert = colors[gl_InstanceID];
}
