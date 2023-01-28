#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coord;
layout (location = 3) in vec3 tangent;

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
  vec3 normal_vert;
  mat3 tbn_mat;
} vs_out;

void main() {
  mat4 model = models[gl_InstanceID];
  mat4 normal_mat = normals_mats[gl_InstanceID];
  gl_Position = projection * view * model * vec4(position, 1.0);

  vs_out.texture_coord_vert = texture_coord;
  vs_out.normal_vert = normalize(mat3(normal_mat) * normal);
  vs_out.position_vert = (model * vec4(position, 1.0)).xyz;

  // re-orthogonalize TBN vectors (bcoz of smoothing by averaging tangents in assimp)
  vec3 tangent_ortho = normalize(tangent - dot(normal, tangent) * normal);
  vec3 bitangent = normalize(cross(normal, tangent_ortho));

  // TBN vectors initially calculated in local space (bitangent=y-axis for cylinder faces)
  vec3 normal_world = normalize(mat3(normal_mat) * normal);
  vec3 tangent_world = normalize(mat3(normal_mat) * tangent_ortho);
  vec3 bitangent_world = normalize(mat3(normal_mat) * bitangent);

  // TBN matrix to transform from tangent to world space
  // https://learnopengl.com/Advanced-Lighting/Normal-Mapping
  vs_out.tbn_mat = mat3(tangent_world, bitangent_world, normal_world);
}
