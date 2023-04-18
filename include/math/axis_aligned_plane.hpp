#ifndef AXIS_ALIGNED_PLANE_HPP
#define AXIS_ALIGNED_PLANE_HPP

#include <glm/glm.hpp>

#include "math/axis_plane.hpp"
#include "math/ray.hpp"

/* Used for defining six planes tangents to axis-aligned bounding box */
namespace math {
  struct AxisAlignedPlane {
    float scalar;
    AxisPlane axis;

    AxisAlignedPlane(float scalar_plane, AxisPlane axis_plane);
    bool intersect_line(const Ray& ray, glm::vec3& intersection_point) const;
  };
}

#endif // AXIS_ALIGNED_PLANE_HPP
