#include "math/plane.hpp"

using namespace math;

/**
 * Plane of equation ax + by + cy = d, with:
 * - normal = (x, y, z), must be normalized
 * - d: the *signed* orthogonal distance from origin to plane
 *     + d > 0: if the origin lies on the opposite side to the dir. of the normal
 *     + d < 0: otherwise (source: https://stackoverflow.com/a/13434753/2228912)
 *     + d = 0: plane passes through the origin
 */
Plane::Plane(const glm::vec3& n, const glm::vec3& point):
  normal(glm::normalize(n)),
  d(glm::dot(normal, point))
{}

/**
 * <p, normal> - d:
 *     + > 0: if point p in same side the normal is pointing to (in front of plane)
 *     + < 0: if point p behind plane
 *     + = 0: if point belongs to plane
 * source: https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling
 */
float Plane::get_signed_distance(const glm::vec3& point) const {
  return glm::dot(normal, point) - d;
}

/* True if point located on same side plane normal is pointing to */
bool Plane::is_in_front_of_plane(const glm::vec3& point) const {
  return get_signed_distance(point) > 0;
}

/**
 * True if parts or all of bbox half-extent is on plane's normal side
 * https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling
 */
bool Plane::is_in_front_of_plane(const BoundingBox& bbox) const {
  // Fig 9.12 & Eq 9.25: http://what-when-how.com/advanced-methods-in-computer-graphics/collision-detection-advanced-methods-in-computer-graphics-part-3/
  // radius = largest projected distance by any AABB vertex (i.e closest one to plane) on plane normal
  // abs. to ensure normal points in same dir. as bbox's half-diag (=> max dist.)
  float radius_bbox = glm::dot(bbox.half_diagonal, glm::abs(normal));

  return get_signed_distance(bbox.center) > -radius_bbox;
}
