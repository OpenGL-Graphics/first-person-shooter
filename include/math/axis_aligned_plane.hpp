#ifndef AXIS_ALIGNED_PLANE_HPP
#define AXIS_ALIGNED_PLANE_HPP

#include <glm/glm.hpp>

/* Used for defining six planes tangents to axis-aligned bounding box */
// TODO: Improve implementation: A float and an enum to specify the axis
namespace math {
  struct AxisAlignedPlane {
    float* x0;
    float* y0;
    float* z0;

    AxisAlignedPlane(float* x, float* y, float* z);
    bool intersect_line(const glm::vec3& point, const glm::vec3& vector, glm::vec3& intersection_point) const;
  };
}

#endif // AXIS_ALIGNED_PLANE_HPP
