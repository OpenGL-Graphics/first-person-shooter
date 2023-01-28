#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

#define MAX_N_INSTANCES 10

struct Light {
  vec3 position;
  vec3 ambiant;
  vec3 diffuse;
  vec3 specular;
};

// opengl tranformation matrices
// array cannot have a dyamic size in glsl
// works if a smaller-sized array (size < max) is passed from the cpu host
uniform mat4 models[MAX_N_INSTANCES]; // object coord -> world coord
uniform mat4 view; // world coord  -> camera coord
uniform mat4 projection; // camera coord -> ndc coord

uniform Light lights[MAX_N_INSTANCES];
uniform mat4 normals_mats[MAX_N_INSTANCES];

out vec3 position_vert;
out vec3 normal_vert;
out Light light_vert;
out mat4 normal_mat_vert;

void main() {
  mat4 model = models[gl_InstanceID];
  gl_Position = projection * view * model * vec4(position, 1.0);

  // fragment position is in world space (same as light)
  position_vert = (model * vec4(position, 1.0)).xyz;
  normal_vert = normal;

  // pass instance's uniforms bcos gl_InstanceID doesn't exist in frag shader
  light_vert = lights[gl_InstanceID];
  normal_mat_vert = normals_mats[gl_InstanceID];
}
