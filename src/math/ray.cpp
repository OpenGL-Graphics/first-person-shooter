#include "math/ray.hpp"

/**
 * Direction doesn't have to be a unit vector (for now)
 * @param origin Camera position
 * @param direction Camera's look direction
 */
Ray::Ray(const glm::vec3& orig, const glm::vec3& dir):
  origin(orig),
  direction(dir)
{}
