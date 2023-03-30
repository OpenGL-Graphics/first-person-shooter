#ifndef AXIS_ALIGNED_PLANE_HPP
#define AXIS_ALIGNED_PLANE_HPP

#include <glm/glm.hpp>

/* Used for defining six planes tangents to axis-aligned bounding box */
namespace math {
  struct AxisAlignedPlane {
    float* x0;
    float* y0;
    float* z0;

    AxisAlignedPlane(float* x, float* y, float* z);
    glm::vec3 intersect_line(const glm::vec3& point, const glm::vec3& vector) const;
  };
}

#endif // AXIS_ALIGNED_PLANE_HPP
