#version 460 core

#define MAX_N_INSTANCES 10

// different colors for each material & light components
struct Material {
  vec3 ambiant;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

struct Light {
  vec3 position;
  vec3 ambiant;
  vec3 diffuse;
  vec3 specular;
};

in vec3 normal_vert;
in vec3 position_vert;
in Light light_vert;
in mat4 normal_mat_vert;

uniform Material material;
uniform vec3 position_camera;

out vec4 color_out;

void main() {
  // can't transform normal vec to world-coord using model matrix
  // https://learnopengl.com/Lighting/Basic-Lighting
  vec3 normal = normalize(mat3(normal_mat_vert) * normal_vert);

  // ambiant light constant
  Light light = light_vert;
  vec3 ambiant = material.ambiant * light.ambiant;

  // diffuse light depends on light beam & fragment normal
  vec3 normal_vec = normalize(normal);
  vec3 light_vec = normalize(light.position - position_vert);
  float strength_diffuse = max(dot(light_vec, normal_vec), 0.0);
  vec3 diffuse = (strength_diffuse * material.diffuse) * light.diffuse;

  // specular light depends on reflected light beam on fragment & camera position
  vec3 camera_vec = normalize(position_camera - position_vert);
  vec3 light_reflect_vec = reflect(-light_vec, normal_vec);
  float strength_specular = pow(max(dot(light_reflect_vec, camera_vec), 0.0), material.shininess);
  vec3 specular = (strength_specular * material.specular) * light.specular;

  // combine material components
  color_out = vec4(ambiant + diffuse + specular, 1.0);
}
