#ifndef BOUNDING_BOX_HPP
#define BOUNDING_BOX_HPP

#include <vector>
#include <glm/glm.hpp>

struct BoundingBox {
  glm::vec3 min;
  glm::vec3 max;
  static const int NO_COLLISION = -1;

  BoundingBox();
  BoundingBox(const std::vector<glm::vec3>& positions);
  void transform(const glm::mat4& mat_model);

  bool check_collision(const BoundingBox& bounding_box);
  int check_collision(const std::vector<BoundingBox>& bounding_boxes);
};

#endif // BOUNDING_BOX_HPP
