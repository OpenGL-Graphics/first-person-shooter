#include "math/transformation.hpp"

/**
 * Model matrix not needed as we'll never convert back to local space
 * Projection corresponds to projection 3D matrix (not 2D one)
 */
Transformation::Transformation(const glm::mat4& v, const glm::mat4& p, float z):
  m_view(v),
  m_projection(p),
  m_z(z)
{
}

/**
 * Convert 2D vector from clip space (NDC) to world space
 * Used for raycasting (i.e. intersection of 2D mouse cursor with 3D scene)
 * @param position_ndc XY coords in [-1, 1]
 * @return position_world Normalized vector in world space
 */
glm::vec4 Transformation::transform_inv(const glm::vec2& position_ndc) {
  // glm::vec4 position_view = glm::inverse(m_projection) * glm::vec4(position_ndc, m_z, 1.0f);
  glm::vec4 position_view = glm::inverse(m_projection) * glm::vec4(position_ndc, -1.0f, 1.0f);
  glm::vec4 position_world = glm::inverse(m_view) * position_view;
  // position_world = glm::normalize(position_world);

  return position_world;
}
