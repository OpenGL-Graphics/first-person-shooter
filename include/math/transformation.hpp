#ifndef TRANSFORMATION_HPP
#define TRANSFORMATION_HPP

#include <glm/glm.hpp>

/* Aggregator for transformation matrices (to avoid pass 3 params to `Renderer`) */
struct Transformation {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 projection;

  Transformation();
  Transformation(const glm::mat4& m, const glm::mat4& v, const glm::mat4& p);

  // glm::vec4 transform_inv(const glm::vec2& position_ndc);
};

#endif // TRANSFORMATION_HPP
