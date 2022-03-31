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

// texture for plane
uniform sampler2D texture2d;

out vec4 color_out;

void main() {
  // texture for fragment
  vec4 color = texture(texture2d, texture_coord_vert);

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
