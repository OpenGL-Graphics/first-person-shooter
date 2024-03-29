#include <algorithm>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

#include "math/axis_aligned_plane.hpp"
#include "math/bounding_box.hpp"

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
 * Line defined in 3d by parameteric equations: x = x0 + a*t, y = y0 + b*t, z = z0 + c*t
 * Two parameters below define 3D line corresp. to camera's line of sight
 * @param ray Camera look direction
 */
bool BoundingBox::intersects(const Ray& ray) {
  // six AA-Planes tangent to bbox faces
  AxisAlignedPlane plane_minx(min.x, AxisPlane::YZ);
  AxisAlignedPlane plane_maxx(max.x, AxisPlane::YZ);
  AxisAlignedPlane plane_miny(min.y, AxisPlane::XZ);
  AxisAlignedPlane plane_maxy(max.y, AxisPlane::XZ);
  AxisAlignedPlane plane_minz(min.z, AxisPlane::XY);
  AxisAlignedPlane plane_maxz(max.z, AxisPlane::XY);
  std::vector<AxisAlignedPlane> aa_planes = {plane_minx, plane_maxx, plane_miny, plane_maxy, plane_minz, plane_maxz};

  for (const AxisAlignedPlane& aa_plane : aa_planes) {
    // find intersection points between line and bounding box mathematical (unbounded) plane
    glm::vec3 intersect;
    bool is_plane_behind_ray = !aa_plane.intersect_line(ray, intersect);

    // bbox plane behind the ray
    if (is_plane_behind_ray) {
      std::cout << " Plane behind ray" << '\n';
      continue;
    }

    // check if intersection point belongs to bounding box face rectangle
    switch (aa_plane.axis) {
      case AxisPlane::YZ: // planes x = minx and x = maxx
        if ((intersect.y >= min.y && intersect.y <= max.y) &&
            (intersect.z >= min.z && intersect.z <= max.z)) {
          std::cout << " inside" << '\n';
          return true;
        }
        break;

      case AxisPlane::XZ: // planes y = miny and y = maxy
        if ((intersect.x >= min.x && intersect.x <= max.x) &&
            (intersect.z >= min.z && intersect.z <= max.z)) {
          std::cout << " inside" << '\n';
          return true;
        }
        break;

      case AxisPlane::XY: // planes z = minz and z = maxz
        if ((intersect.x >= min.x && intersect.x <= max.x) &&
            (intersect.y >= min.y && intersect.y <= max.y)) {
          std::cout << " inside" << '\n';
          return true;
        }
    }

    std::cout << " outside" << '\n';
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

  // 90deg vertical wall rotation => min.z = max_z (& max.z = min_z) => recalculate
  glm::vec3 min_temp(std::min(min.x, max.x), std::min(min.y, max.y), std::min(min.z, max.z));
  glm::vec3 max_temp(std::max(min.x, max.x), std::max(min.y, max.y), std::max(min.z, max.z));

  min = min_temp;
  max = max_temp;

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
