#ifndef MATH_PLANE_HPP
#define MATH_PLANE_HPP

#include <glm/glm.hpp>

/* Plane defined by its normal vector and the signed dist from origin d */
namespace math {
  struct Plane {
    glm::vec3 normal;
    float d;

    Plane() = default;
    Plane(const glm::vec3& n, const glm::vec3& point);
    bool is_in_front_of_plane(const glm::vec3& point);
  };
}

#endif // MATH_PLANE_HPP
