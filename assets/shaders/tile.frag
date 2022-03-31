#version 330 core

in vec2 texture_coord_vert;
in vec3 position_vert;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;

uniform vec3 position_light;
uniform mat3 normal_mat; // normal matrix (inverse() not defined in glsl-1.30)

out vec4 color_out;

/* modified from `assets/texture_surface.frag` */
void main() {
  vec4 color = texture(texture_diffuse, texture_coord_vert);

  // normal-mapping based shading: convert image from [0, 1] to [-1, 1]
  // https://learnopengl.com/Advanced-Lighting/Normal-Mapping
  vec3 normal = texture(texture_normal, texture_coord_vert).rgb;
  vec3 normal_vec = normalize(normal * 2.0 - 1.0);

  // can't transform normal vec to world-coord using model matrix
  // https://learnopengl.com/Lighting/Basic-Lighting
  // normal_vec = normalize(normal_mat * normal_vec);

  // ambiant
  vec3 ambiant = 0.75 * color.xyz;

  // diffuse (normal vector from texture image)
  vec3 light_vec = normalize(position_light - position_vert);
  float strength_diffuse = max(dot(light_vec, normal_vec), 0.0);
  vec3 diffuse = 0.5 * strength_diffuse * color.xyz;

  // combine ambiant & diffuse contributions
  color_out = vec4(ambiant + diffuse, 1.0);
}
