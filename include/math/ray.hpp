#ifndef RAY_HPP
#define RAY_HPP

#include <glm/glm.hpp>

struct Ray {
  glm::vec3 origin;
  glm::vec3 direction;

  Ray(const glm::vec3& orig, const glm::vec3& dir);
};

#endif // RAY_HPP
