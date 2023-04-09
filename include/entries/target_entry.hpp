#ifndef TARGET_ENTRY_HPP
#define TARGET_ENTRY_HPP

#include <glm/glm.hpp>

#include "math/bounding_box.hpp"

struct TargetEntry {
  bool is_dead;
  glm::vec3 position;
  BoundingBox bounding_box;
};

#endif // TARGET_ENTRY_HPP
