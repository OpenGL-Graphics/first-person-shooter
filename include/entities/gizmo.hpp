#ifndef GIZMO_HPP
#define GIZMO_HPP

#include "program.hpp"
#include "vertexes/attribute.hpp"
#include "render/renderer.hpp"

/* xyz axes drawn at origin */
class Gizmo {
public:
  Gizmo(const Program& program);
  void set_transform(const Transformation& t);
  void draw();
  void free();

private:
  const std::vector<Attribute> ATTRIBUTES = { {0, "position", 3, 3, 0} };
  const glm::vec3 RED = { 1.0f, 0.0f, 0.0f };
  const glm::vec3 GREEN = { 0.0f, 1.0f, 0.0f };
  const glm::vec3 BLUE = { 0.0f, 0.0f, 1.0f };

  Renderer m_x_axis;
  Renderer m_y_axis;
  Renderer m_z_axis;
};

#endif // GIZMO_HPP
