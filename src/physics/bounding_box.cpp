#include <algorithm>
#include <iostream>

#include "physics/bounding_box.hpp"
#include "math/axis_aligned_plane.hpp"

using namespace math;

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
 * Check if 3D line crosses bounding box
 * Intersection point between 3D line & plane: https://www.youtube.com/watch?v=Td9CZGkqrSg
 * Line defined in 3d by parameteric equations: x = x0 + ta, y = y0 + ty, z = z0 = tz
 * Two parameters below define 3D line corresp. to camera's line of sight
 * @param point Camera position
 * @param vector Camera's look direction
 */
bool BoundingBox::intersects(const glm::vec3& point, const glm::vec3& vector) {
  // six AA-Planes tangent to bbox faces
  AxisAlignedPlane plane_minx(&min.x, nullptr, nullptr);
  AxisAlignedPlane plane_maxx(&max.x, nullptr, nullptr);
  AxisAlignedPlane plane_miny(nullptr, &min.y, nullptr);
  AxisAlignedPlane plane_maxy(nullptr, &max.y, nullptr);
  AxisAlignedPlane plane_minz(nullptr, nullptr, &min.z);
  AxisAlignedPlane plane_maxz(nullptr, nullptr, &max.z);
  std::vector<AxisAlignedPlane> aa_planes = {plane_minx, plane_maxx, plane_miny, plane_maxy, plane_minz, plane_maxz};

  for (const AxisAlignedPlane& aa_plane : aa_planes) {
    // find intersection points between line and bounding box mathematical (unbounded) plane
    glm::vec3 intersection_point = aa_plane.intersect_line(point, vector);

    // check if intersection point belongs to bounding box face rectangle
    if (aa_plane.y0 == nullptr && aa_plane.z0 == nullptr) { // planes x = minx and x = maxx
      if ((intersection_point.y >= min.y && intersection_point.y <= max.y) &&
          (intersection_point.z >= min.z && intersection_point.z <= max.z)) {
        return true;
      }
    }

    if (aa_plane.x0 == nullptr && aa_plane.z0 == nullptr) { // planes y = miny and y = maxy
      if ((intersection_point.x >= min.x && intersection_point.x <= max.x) &&
          (intersection_point.z >= min.z && intersection_point.z <= max.z)) {
        return true;
      }
    }

    if (aa_plane.x0 == nullptr && aa_plane.y0 == nullptr) { // planes z = minz and z = maxz
      if ((intersection_point.x >= min.x && intersection_point.x <= max.x) &&
          (intersection_point.y >= min.y && intersection_point.y <= max.y)) {
        return true;
      }
    }
  }

  return false;
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
