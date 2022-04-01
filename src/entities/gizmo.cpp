#include "entities/gizmo.hpp"
#include "geometries/line.hpp"

Gizmo::Gizmo(const Program& program):
  m_x_axis(program, VBO(Line({ 0, 0, 0 }, { 1, 0, 0 })), ATTRIBUTES),
  m_y_axis(program, VBO(Line({ 0, 0, 0 }, { 0, 1, 0 })), ATTRIBUTES),
  m_z_axis(program, VBO(Line({ 0, 0, 0 }, { 0, 0, 1 })), ATTRIBUTES)
{
}

void Gizmo::set_transform(const Transformation& transformation) {
  m_x_axis.set_transform(transformation);
  m_y_axis.set_transform(transformation);
  m_z_axis.set_transform(transformation);
}

void Gizmo::draw() {
  m_x_axis.draw({ {"color", RED} }, GL_LINES);
  m_y_axis.draw({ {"color", GREEN} }, GL_LINES);
  m_z_axis.draw({ {"color", BLUE} }, GL_LINES);
}

void Gizmo::free() {
  m_x_axis.free();
  m_y_axis.free();
  m_z_axis.free();
}
