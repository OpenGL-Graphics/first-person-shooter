#version 130

in vec3 normal_vert;
in vec3 position_vert;

uniform vec3 color;
uniform vec3 color_light;
uniform vec3 position_light;
uniform vec3 position_camera;

out vec4 color_out;

void main() {
  // ambiant light
  float strength_ambiant = 0.1;
  vec3 ambiant = strength_ambiant * color_light;

  // diffuse light
  vec3 light = normalize(position_light - position_vert);
  float strength_diffuse = max(dot(light, normal_vert), 0.0);
  vec3 diffuse = strength_diffuse * color_light;

  // specular light
  vec3 camera = normalize(position_camera - position_vert);
  vec3 light_reflect = reflect(-light, normal_vert);
  float strength_specular = 0.5 * pow(max(dot(light_reflect, camera), 0.0), 32);
  vec3 specular = strength_specular * color_light;

  // combine lights with object color
  color_out = vec4((ambiant + diffuse + specular) * color, 1.0);
}
