#include "globals/lights.hpp"

// inside room 1
Light light1(
  glm::vec3(5.5f, 3.0f, 4.0f),
  glm::vec3(1)
);

// between two rooms
Light light2(
  glm::vec3(16.0f, 3.0f, 4.0f),
  glm::vec3(1)
);

// inside room 2
Light light3(
  glm::vec3(26.5f, 3.0f, 4.0f),
  glm::vec3(1)
);

// global variable
// use an std::array to get its length inside for loop in main.cpp
std::array<Light, 3> lights = { light1, light2, light3 };
