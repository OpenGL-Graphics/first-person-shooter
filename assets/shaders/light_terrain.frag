#version 330 core

// light components
struct Light {
  vec3 position;
  vec3 ambiant;
  vec3 diffuse;
  vec3 specular;
};

// input coords passed from vertex shader
in vec3 normal_vert;
in vec3 position_vert;
in vec2 texture_coord_vert;

// light-specific
uniform Light light;

// multiply textures for grass/rock/water/sand
uniform sampler2D texture2d_water;
uniform sampler2D texture2d_grass;
uniform sampler2D texture2d_rock;
uniform sampler2D texture2d_splatmap;

out vec4 color_out;

/* Functions declarations */
vec4 texture_by_splatmap(vec4 color_water, vec4 color_grass, vec4 color_rock, vec4 color_splatmap);

void main() {
  // textures
  vec4 color_water = texture(texture2d_water, texture_coord_vert);
  vec4 color_grass = texture(texture2d_grass, texture_coord_vert);
  vec4 color_rock = texture(texture2d_rock, texture_coord_vert);
  vec4 color_splatmap = texture(texture2d_splatmap, texture_coord_vert);
  vec4 color = texture_by_splatmap(color_water, color_grass, color_rock, color_splatmap);

  // ambiant light constant
  vec3 ambiant = color.xyz * light.ambiant;

  // diffuse light depends on light beam & fragment normal
  vec3 normal_vec = normalize(normal_vert);
  vec3 light_vec = normalize(light.position - position_vert);
  float strength_diffuse = max(dot(light_vec, normal_vec), 0.0);
  vec3 diffuse = (strength_diffuse * color.xyz) * light.diffuse;

  // combine material components
  color_out = vec4(ambiant + diffuse, 1.0);
}

/**
 * Choose pixel color accord. to rgb values from splatmap texture
 * In splatmap: red (most elevated) => rock, green => grass, blue (lowest) => water
 * Inspired by: https://stackoverflow.com/a/1111089/2228912
 */
vec4 texture_by_splatmap(vec4 color_water, vec4 color_grass, vec4 color_rock, vec4 color_splatmap) {
  // rgb values allow to calculate weights of each texture in resulting pixel value
  float pixel_rock = color_splatmap.r;
  float pixel_grass = color_splatmap.g;
  float pixel_water = color_splatmap.b;
  float pixel_sum = pixel_rock + pixel_grass + pixel_water;

  // weight are ratios of rgb components values relative to their sum
  float percentage_rock = pixel_rock / pixel_sum;
  float percentage_grass = pixel_grass / pixel_sum;
  float percentage_water = pixel_water / pixel_sum;
  vec4 result = percentage_rock * color_rock + percentage_grass * color_grass + percentage_water * color_water;

  return result;
}
