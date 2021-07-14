#ifndef BOUNDING_BOX_HPP
#define BOUNDING_BOX_HPP

#include <vector>
#include <glm/glm.hpp>

struct BoundingBox {
  glm::vec3 min;
  glm::vec3 max;

  BoundingBox();
  BoundingBox(const std::vector<glm::vec3>& positions);
  void transform(const glm::mat4& mat_model);
  bool check_collision(const BoundingBox& bounding_box);
};

#endif // BOUNDING_BOX_HPP
