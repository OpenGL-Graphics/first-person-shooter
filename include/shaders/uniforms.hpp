#ifndef UNIFORMS_HPP
#define UNIFORMS_HPP

#include <glm/glm.hpp>
#include <materials/texture.hpp>
#include <string>

struct Uniforms {
  // tranformation matrices
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 projection;

  // material
  glm::vec3 color;
  glm::vec3 material_color;
  glm::vec3 material_ambiant;
  glm::vec3 material_diffuse;
  glm::vec3 material_specular;
  float material_shininess;

  // light
  glm::vec3 light_position;
  glm::vec3 light_ambiant;
  glm::vec3 light_diffuse;
  glm::vec3 light_specular;

  // camera
  glm::vec3 camera_position;

  // texture
  int texture2d;
  int texture3d;
};

#endif // UNIFORMS_HPP
