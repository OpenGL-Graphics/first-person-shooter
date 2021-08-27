#ifndef TRANSFORMATION_HPP
#define TRANSFORMATION_HPP

#include <glm/glm.hpp>

/* See raycasting tutorial: https://antongerdelan.net/opengl/raycasting.html */
class Transformation {
public:
  Transformation(const glm::mat4& view, const glm::mat4& projection, float z);
  glm::vec4 transform_inv(const glm::vec2& position_ndc);

private:
  glm::mat4 m_view;
  glm::mat4 m_projection;
  float m_z;
};

#endif // TRANSFORMATION_HPP
