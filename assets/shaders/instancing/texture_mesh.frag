#version 460 core

#define N_LIGHTS 3

// interface block (name matches in vertex shader)
in VS_OUT {
  vec2 texture_coord_vert;
  vec3 position_vert;
  vec3 normal_vert;
  mat3 tbn_mat;
} fs_in;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform vec3 positions_lights[N_LIGHTS];
uniform vec3 position_camera;

// tree meshes have diffuse colors but no textures (get normals from vertexes)
uniform bool has_texture_diffuse;
uniform bool has_texture_normal;
uniform vec3 color;

out vec4 color_out;

/* functions declarations */
vec3 calculateLightContribution(vec4 color, vec3 position_light, vec3 normal);

/* modified from `assets/texture_surface.frag` */
void main() {
  vec4 color_texture = (has_texture_diffuse) ? texture(texture_diffuse, fs_in.texture_coord_vert) : vec4(color, 1.0);
  vec3 normal = fs_in.normal_vert;

  if (has_texture_normal) {
    // normal-mapping based shading: convert image from [0, 1] to [-1, 1]
    // normal vector from texture image: https://learnopengl.com/Advanced-Lighting/Normal-Mapping
    vec3 normal_vec = texture(texture_normal, fs_in.texture_coord_vert).rgb;
    normal_vec = normalize(normal_vec * 2.0 - 1.0);

    // TBN matrix to transform from tangent to world space
    // https://learnopengl.com/Advanced-Lighting/Normal-Mapping
    normal = normalize(mat3(fs_in.tbn_mat) * normal_vec);
  }

  // sum-up color contributions from all walls
  vec3 sum_contributions = vec3(0.0, 0.0, 0.0);
  for (int i_light = 0; i_light < N_LIGHTS; i_light++) {
    sum_contributions += calculateLightContribution(color_texture, positions_lights[i_light], normal);
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
  vec3 light_vec = normalize(position_light - fs_in.position_vert);
  float strength_diffuse_pos = max(dot(light_vec, normal_vec), 0.0);
  float strength_diffuse_neg = max(dot(light_vec, normal_vec_neg), 0.0);
  float strength_diffuse = strength_diffuse_pos;

  if (strength_diffuse_pos < strength_diffuse_neg) {
    strength_diffuse = strength_diffuse_neg;
    normal_vec = normal_vec_neg;
  }

  vec3 diffuse = 1.0 * strength_diffuse * color.xyz;

  // specular light depends on reflected light beam on fragment & camera position
  vec3 camera_vec = normalize(position_camera - fs_in.position_vert);
  vec3 light_reflect_vec = reflect(-light_vec, normal_vec);
  // float shininess = 32.0;
  float shininess = 4.0;
  float strength_specular = pow(max(dot(light_reflect_vec, camera_vec), 0.0), shininess);
  vec3 specular = 0.5 * strength_specular * color.xyz;

  // attenuation: https://learnopengl.com/Lighting/Light-casters
  float dist = length(position_light - fs_in.position_vert);
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
