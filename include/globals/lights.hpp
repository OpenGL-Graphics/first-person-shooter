#ifndef LIGHTS_HPP
#define LIGHTS_HPP

#include <glm/glm.hpp>

struct Light {
  glm::vec3 position;
  glm::vec3 color;

  Light(const glm::vec3& p, const glm::vec3& c);
};

// score declared as a global variable (i.e. accessible everywhere)
// https://stackoverflow.com/a/3627979/2228912 
extern Light lights[3];

#endif // LIGHTS_HPP
