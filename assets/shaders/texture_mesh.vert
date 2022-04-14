#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coord;
/*
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
*/

// opengl tranformation matrices
uniform mat4 model;      // object coord -> world coord
uniform mat4 view;       // world coord  -> camera coord
uniform mat4 projection; // camera coord -> ndc coord
uniform mat4 normal_mat;

// interface block (name matches in frag shader)
out VS_OUT {
  vec2 texture_coord_vert;
  vec3 position_vert;
  vec3 normal_vert;
  mat3 tbn_mat;
} vs_out;

void main() {
  gl_Position = projection * view * model * vec4(position, 1.0);

  vs_out.texture_coord_vert = texture_coord;
  vs_out.normal_vert = normal;
  vs_out.position_vert = (model * vec4(position, 1.0)).xyz;

  // TBN vectors initially calculated in local space (bitangent=y-axis for cylinder faces)
  vec3 normal_world = normalize(mat3(normal_mat) * normal);
  vec3 bitangent_world = normalize(mat3(normal_mat) * vec3(0.0, 1.0, 0.0));
  vec3 tangent_world = normalize(cross(bitangent_world, normal_world));

  // TBN matrix to transform from tangent to world space
  // https://learnopengl.com/Advanced-Lighting/Normal-Mapping
  vs_out.tbn_mat = mat3(tangent_world, bitangent_world, normal_world);
}
