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
bool Plane::is_in_front_of_plane(const glm::vec3& point) const {
  return glm::dot(normal, point) - d > 0;
}
