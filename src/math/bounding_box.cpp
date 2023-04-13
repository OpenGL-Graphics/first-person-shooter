#include <algorithm>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

#include "math/bounding_box.hpp"
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

  calculate_min_max(positions);
  calculate_center_diagonal();
}

/* Used to check intersec. of windows with Frustum */
BoundingBox::BoundingBox(const glm::vec3& c, const glm::vec3& hd):
  center(c),
  half_diagonal(hd)
{
  min = center - half_diagonal;
  max = center + half_diagonal;
}

void BoundingBox::calculate_min_max(const std::vector<glm::vec3>& positions) {
  auto get_x_bound = [](const glm::vec3& position1, const glm::vec3& position2) { return position1.x < position2.x; };
  auto get_y_bound = [](const glm::vec3& position1, const glm::vec3& position2) { return position1.y < position2.y; };
  auto get_z_bound = [](const glm::vec3& position1, const glm::vec3& position2) { return position1.z < position2.z; };

  glm::vec3 min_x = *std::min_element(positions.begin(), positions.end(), get_x_bound);
  glm::vec3 min_y = *std::min_element(positions.begin(), positions.end(), get_y_bound);
  glm::vec3 min_z = *std::min_element(positions.begin(), positions.end(), get_z_bound);

  glm::vec3 max_x = *std::max_element(positions.begin(), positions.end(), get_x_bound);
  glm::vec3 max_y = *std::max_element(positions.begin(), positions.end(), get_y_bound);
  glm::vec3 max_z = *std::max_element(positions.begin(), positions.end(), get_z_bound);

  min = glm::vec3(min_x.x, min_y.y, min_z.z);
  max = glm::vec3(max_x.x, max_y.y, max_z.z);
}

/* Calculate bbox center & its half diagonal (must be called after calc. min/max) */
void BoundingBox::calculate_center_diagonal() {
  center = (min + max) / 2.0f;
  half_diagonal = max - center;
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
  calculate_center_diagonal();
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

/* For debugging */
std::ostream& operator<<(std::ostream& stream, const BoundingBox& bbox) {
  return stream << "Center: " << glm::to_string(bbox.center)
                << " min: " << glm::to_string(bbox.min)
                << " max: " << glm::to_string(bbox.max);
}
