#ifndef BOUNDING_BOX_HPP
#define BOUNDING_BOX_HPP

#include <ostream>
#include <vector>
#include <glm/glm.hpp>

#include "math/ray.hpp"

struct BoundingBox {
  static const int NO_COLLISION = -1;

  glm::vec3 min;
  glm::vec3 max;

  /* needed for frustum culling */
  glm::vec3 center;
  glm::vec3 half_diagonal;

  BoundingBox();
  BoundingBox(const std::vector<glm::vec3>& positions);
  BoundingBox(const glm::vec3& c, const glm::vec3& hd);
  void transform(const glm::mat4& mat_model);

  bool check_collision(const BoundingBox& bounding_box);
  int check_collision(const std::vector<BoundingBox>& bounding_boxes);
  bool intersects(const Ray& ray);

  /* Friend: non-member function that has access to class' private fields */
  friend std::ostream& operator<<(std::ostream& stream, const BoundingBox& bbox);

private:
  void calculate_min_max(const std::vector<glm::vec3>& positions);
  void calculate_center_diagonal();
};

#endif // BOUNDING_BOX_HPP
