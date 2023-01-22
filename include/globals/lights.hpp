#ifndef LIGHTS_HPP
#define LIGHTS_HPP

#include <glm/glm.hpp>
#include <array>

struct Light {
  glm::vec3 position;
  glm::vec3 color; // needed to show light source
  glm::vec3 ambiant;
  glm::vec3 diffuse;
  glm::vec3 specular;

  Light(const glm::vec3& p, const glm::vec3& c);
};

// lights declared as a global variable (i.e. same instance accessible everywhere)
// https://stackoverflow.com/a/3627979/2228912 
extern std::array<Light, 3> lights;

#endif // LIGHTS_HPP
