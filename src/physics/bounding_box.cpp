#include <algorithm>
#include <iostream>

#include "physics/bounding_box.hpp"

/* Needed so bbox (class member) can be automatically init in Renderer constructor */
BoundingBox::BoundingBox() {
}

BoundingBox::BoundingBox(const std::vector<glm::vec3>& positions) {
  if (positions.size() == 0) {
    // std::cout << "Empty positions vector" << '\n';
    return;
  }

  glm::vec3 min_x = *std::min_element(positions.begin(), positions.end(), [](const glm::vec3& position1, const glm::vec3& position2) {
    return position1.x < position2.x;
  });
  glm::vec3 min_y = *std::min_element(positions.begin(), positions.end(), [](const glm::vec3& position1, const glm::vec3& position2) {
    return position1.y < position2.y;
  });
  glm::vec3 min_z = *std::min_element(positions.begin(), positions.end(), [](const glm::vec3& position1, const glm::vec3& position2) {
    return position1.z < position2.z;
  });

  glm::vec3 max_x = *std::max_element(positions.begin(), positions.end(), [](const glm::vec3& position1, const glm::vec3& position2) {
    return position1.x < position2.x;
  });
  glm::vec3 max_y = *std::max_element(positions.begin(), positions.end(), [](const glm::vec3& position1, const glm::vec3& position2) {
    return position1.y < position2.y;
  });
  glm::vec3 max_z = *std::max_element(positions.begin(), positions.end(), [](const glm::vec3& position1, const glm::vec3& position2) {
    return position1.z < position2.z;
  });

  min = glm::vec3(min_x.x, min_y.y, min_z.z);
  max = glm::vec3(max_x.x, max_y.y, max_z.z);
}

/**
 * Apply given model (tranformation) matrix to bounding box
 * Updates bounding box when 3D object is moved 
 */
void BoundingBox::transform(const glm::mat4& mat_model) {
  min = mat_model * glm::vec4(min, 1.0f);
  max = mat_model * glm::vec4(max, 1.0f);
}

/* Check if current bounding box collides with given one */
bool BoundingBox::check_collision(const BoundingBox& bounding_box) {
  if (max.x < bounding_box.min.x || min.x > bounding_box.max.x)
    return false;
  if (max.y < bounding_box.min.y || min.y > bounding_box.max.y)
    return false;
  if (max.z < bounding_box.min.z || min.z > bounding_box.max.z)
    return false;

  return true;
}

/**
 * Check for collision against multiple objects
 * @param bounding_boxes Bounding boxes to check for collision against
 * @return index of collided bbox or -1 for no collision
 */
int BoundingBox::check_collision(const std::vector<BoundingBox>& bounding_boxes) {
  for (size_t i_bbox = 0; i_bbox < bounding_boxes.size(); ++i_bbox) {
    if (check_collision(bounding_boxes[i_bbox])) {
        return i_bbox;
    }
  }

  return NO_COLLISION;
}
