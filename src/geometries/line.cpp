#include "geometries/line.hpp"

Line::Line(const glm::vec3& point_start, const glm::vec3& point_end):
  m_point_start(point_start),
  m_point_end(point_end)
{
  set_vertexes();
  set_indices();
  set_n_elements();
}

void Line::set_vertexes() {
  m_vertexes = {
    m_point_start.x, m_point_start.y, m_point_start.z,
    m_point_end.x, m_point_end.y, m_point_end.z,
  };
}

void Line::set_indices() {
  m_indices = {
    0,
    1,
  };
}

void Line::set_n_elements() {
  m_n_elements = 2;
}
