#include "globals/lights.hpp"

Light::Light(const glm::vec3& p, const glm::vec3& c):
  position(p),
  color(c)
{
}

// inside room 1
Light light1(
  glm::vec3( 5.5f, 3.0f, 4.0f),
  glm::vec3(1.0f)
);

// between two rooms
Light light2(
  glm::vec3(16.0f, 3.0f, 4.0f),
  glm::vec3(1.0f)
);

// inside room 2
Light light3(
  glm::vec3(26.5f, 3.0f, 4.0f),
  glm::vec3(1.0f)
);

// global variable
Light lights[3] = { light1, light2, light3 };
