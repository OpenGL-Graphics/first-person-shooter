#version 330 core

in vec2 texture_coord_vert;
in vec3 position_vert;
in vec3 normal_vert;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform mat4 model;
uniform vec3 positions_lights[3];
uniform vec3 position_camera;

out vec4 color_out;

/* functions declarations */
vec3 calculateLightContribution(vec4 color, vec3 position_light, vec3 normal);

/* modified from `assets/texture_surface.frag` */
void main() {
  vec4 color = texture(texture_diffuse, texture_coord_vert);

  /*
  // normal-mapping based shading: convert image from [0, 1] to [-1, 1]
  // normal vector from texture image: https://learnopengl.com/Advanced-Lighting/Normal-Mapping
  vec3 normal_vec = texture(texture_normal, texture_coord_vert).rgb;
  normal_vec = normalize(normal_vec * 2.0 - 1.0);
  */

  // can't transform normal vec to world-coord using model matrix
  // https://learnopengl.com/Lighting/Basic-Lighting
  mat3 normal_mat = mat3(transpose(inverse(model)));
  vec3 normal = normalize(normal_mat * normal_vert);

  // sum-up color contributions from all walls
  vec3 sum_contributions = vec3(0.0, 0.0, 0.0);
  for (int i_light = 0; i_light < 3; i_light++) {
    sum_contributions += calculateLightContribution(color, positions_lights[i_light], normal);
  }

  // combine ambiant & diffuse contributions
  color_out = vec4(sum_contributions, 1.0);
}

/* Calculate contribution to `color` from light positionned at `position_light` */
// TODO: shadow mapping (avoid adding up light contribs to occluded walls)
vec3 calculateLightContribution(vec4 color, vec3 position_light, vec3 normal) {
  // ambiant
  vec3 ambiant = 0.75 * color.xyz;

  // diffuse (certain walls have normals oriented to outside)
  vec3 normal_vec = normal;
  vec3 normal_vec_neg = -normal;
  vec3 light_vec = normalize(position_light - position_vert);
  float strength_diffuse_pos = max(dot(light_vec, normal_vec), 0.0);
  float strength_diffuse_neg = max(dot(light_vec, normal_vec_neg), 0.0);
  float strength_diffuse = strength_diffuse_pos;

  if (strength_diffuse_pos < strength_diffuse_neg) {
    strength_diffuse = strength_diffuse_neg;
    normal_vec = normal_vec_neg;
  }

  vec3 diffuse = 0.5 * strength_diffuse * color.xyz;

  // specular light depends on reflected light beam on fragment & camera position
  vec3 camera_vec = normalize(position_camera - position_vert);
  vec3 light_reflect_vec = reflect(-light_vec, normal_vec);
  float shininess = 32.0;
  float strength_specular = pow(max(dot(light_reflect_vec, camera_vec), 0.0), shininess);
  vec3 specular = 0.2 * strength_specular * color.xyz;

  // attenuation: https://learnopengl.com/Lighting/Light-casters
  float dist = length(position_light - position_vert);
  float k_linear = 0.09;
  float k_quadratic = 0.032;
  float attenuation = 1.0 / (1.0 + k_linear*dist + k_quadratic*dist*dist);

  ambiant *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;

  // combine ambiant & diffuse contributions
  vec3 color_out = ambiant + diffuse + specular;
  return color_out;
}
