#version 130

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
uniform sampler2D texture2d_sand;
uniform sampler2D texture2d_grass;
uniform sampler2D texture2d_water;
uniform sampler2D texture2d_rock;

out vec4 color_out;

/* Functions declarations */
vec4 texture_by_elevation(float elevation, vec4 color_water, vec4 color_sand, vec4 color_grass, vec4 color_rock);

void main() {
  // textures
  vec4 color_sand = texture(texture2d_sand, texture_coord_vert);
  vec4 color_grass = texture(texture2d_grass, texture_coord_vert);
  vec4 color_water = texture(texture2d_water, texture_coord_vert);
  vec4 color_rock = texture(texture2d_rock, texture_coord_vert);
  float elevation = position_vert.y / 10.0;
  vec4 color = texture_by_elevation(elevation, color_water, color_sand, color_grass, color_rock);

  // ambiant light constant
  vec3 ambiant = color.xyz * light.ambiant;
  // vec3 ambiant = vec3(colormap(position_vert.y / 10.0)) * light.ambiant;

  // diffuse light depends on light beam & fragment normal
  vec3 normal_vec = normalize(normal_vert);
  vec3 light_vec = normalize(light.position - position_vert);
  float strength_diffuse = max(dot(light_vec, normal_vec), 0.0);
  vec3 diffuse = (strength_diffuse * color.xyz) * light.diffuse;
  // vec3 diffuse = (strength_diffuse * vec3(colormap(position_vert.y / 10.0))) * light.diffuse;

  // combine material components
  color_out = vec4(ambiant + diffuse, 1.0);
}

/**
 * Choose pixel color from one of given textures accord. to its elevation
 * @param elevation Elevation (y-coord) in [-1, 1]
 */
vec4 texture_by_elevation(float elevation, vec4 color_water, vec4 color_sand, vec4 color_grass, vec4 color_rock) {
  if (elevation < 0.0) {
    return color_water;
  } else if (elevation < 0.05) {
    // border in-between water & sand
    return mix(color_water, color_sand, 0.5);
  } else if (elevation < 0.25) {
    return color_sand;
  } else if (elevation < 0.30) {
    // border in-between sand & grass
    return mix(color_sand, color_grass, 0.5);
  } else if (elevation < 0.50) {
    return color_grass;
  } else if (elevation < 0.55) {
    // border in-between grass & rock
    return mix(color_grass, color_rock, 0.5);
  } else {
    return color_rock;
  }
}
