#include <iostream>

#include "math/axis_aligned_plane.hpp"

using namespace math;

/* AA-Planes have two coords that are null (i.e. z = z0 or y = y0 or x = x0) */
AxisAlignedPlane::AxisAlignedPlane(float* x, float* y, float* z):
  x0(x),
  y0(y),
  z0(z)
{
}

/**
 * Calculates intersection point of AA-Plane and ray defined by its line's parameteric equation
 * @returns False if plane is behind directed ray (i.e. no intersection)
 */
bool AxisAlignedPlane::intersect_line(const Ray& ray, glm::vec3& intersection_point) const {
  // line (ray) defined by a point and a vector
  glm::vec3 point = ray.origin;
  glm::vec3 vector = ray.direction;
  float t;

  if (z0 != nullptr) {
    t = (*z0 - point.z) / vector.z;
  } else if (y0 != nullptr) {
    t = (*y0 - point.y) / vector.y;
  } else if (x0 != nullptr) {
    t = (*x0 - point.x) / vector.x;
  }
  std::cout << "t: " << t;

  // when t < 0 => plane is behind the ray (so no intersection)
  if (t < 0)
    return false;

  intersection_point = {
    point.x + t * vector.x,
    point.y + t * vector.y,
    point.z + t * vector.z,
  };

  return true;
}
