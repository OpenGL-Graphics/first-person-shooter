#include <iostream>

#include "math/axis_aligned_plane.hpp"

using namespace math;

/**
 * AA-Planes has a main plane to which it's paralle (xy, xz, yz plane),
 * and a scalar: translation offset from the origin
 * Equ: z = scalar (axis=xy) or y = scalar (axis=xz) or x = scalar (axis=yz)
 */
AxisAlignedPlane::AxisAlignedPlane(float scalar_plane, AxisPlane axis_plane):
  scalar(scalar_plane),
  axis(axis_plane)
{}

/**
 * Calculates intersection point of AA-Plane and ray defined by its line's parameteric equation
 * @returns False if plane is behind directed ray (i.e. no intersection)
 */
bool AxisAlignedPlane::intersect_line(const Ray& ray, glm::vec3& intersection_point) const {
  // line (ray) defined by a point and a vector
  glm::vec3 point = ray.origin;
  glm::vec3 vector = ray.direction;
  float t;

  switch (axis) {
    case AxisPlane::XY: // plane z = scalar
      t = (scalar - point.z) / vector.z;
      break;
    case AxisPlane::XZ: // plane y = scalar
      t = (scalar - point.y) / vector.y;
      break;
    case AxisPlane::YZ: // plane x = scalar
      t = (scalar - point.x) / vector.x;
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
