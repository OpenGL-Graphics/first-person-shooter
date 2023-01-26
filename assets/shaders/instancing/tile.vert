#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coord;

#define MAX_N_INSTANCES 10

// opengl tranformation matrices
uniform mat4 models[MAX_N_INSTANCES];      // object coord -> world coord
uniform mat4 view;       // world coord  -> camera coord
uniform mat4 projection; // camera coord -> ndc coord

uniform mat4 normals_mats[MAX_N_INSTANCES];

// interface block (name matches in frag shader)
out VS_OUT {
  vec2 texture_coord_vert;
  vec3 position_vert;
  vec3 normal_vert; // not used! (normal retrieved from `texture_normal`)
  mat4 normal_mat_vert;
} vs_out;

/* modified from `assets/texture_surface.vert` */
void main() {
  mat4 model = models[gl_InstanceID];
  gl_Position = projection * view * model * vec4(position, 0.0, 1.0);

  vs_out.texture_coord_vert = texture_coord;
  vs_out.position_vert = (model * vec4(position, 0.0, 1.0)).xyz;
  vs_out.normal_vert = normal;
  vs_out.normal_mat_vert = normals_mats[gl_InstanceID];
}
