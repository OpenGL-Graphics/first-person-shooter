#include "math/transformation.hpp"

/* Used to initialize transformation as member class of `Renderer` */
Transformation::Transformation() {
}

/**
 * Model matrix not needed as we'll never convert back to local space
 * Projection corresponds to projection 3D matrix (not 2D one)
 */
Transformation::Transformation(const glm::mat4& m, const glm::mat4& v, const glm::mat4& p):
  model(m),
  view(v),
  projection(p)
{
}

/**
 * Convert 2D vector from clip space (NDC) to world space
 * Used for raycasting (i.e. intersection of 2D mouse cursor with 3D scene)
 * @param position_ndc XY coords in [-1, 1]
 * @return position_world Normalized vector in world space
 * Raycasting theory: https://antongerdelan.net/opengl/raycasting.html
 * TODO: Not working yet!
 */
/*
glm::vec4 Transformation::transform_inv(const glm::vec2& position_ndc) {
  // glm::vec4 position_view = glm::inverse(m_projection) * glm::vec4(position_ndc, m_z, 1.0f);
  glm::vec4 position_view = glm::inverse(projection) * glm::vec4(position_ndc, -1.0f, 1.0f);
  glm::vec4 position_world = glm::inverse(view) * position_view;
  // position_world = glm::normalize(position_world);

  return position_world;
}
*/
