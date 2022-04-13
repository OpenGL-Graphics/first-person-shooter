#include <iostream>
#include <cmath>

#include "geometries/cylinder.hpp"

/* Calculate vertexes & indices on creation */
Cylinder::Cylinder(int n_corners):
  m_n_corners(n_corners)
{
  _set_vertexes();
  _set_indices();
}

void Cylinder::_set_vertexes() {
  // center of bottom circle & its corners (then same for top circle)
  float angle_step = 2*M_PI / m_n_corners;

  for (auto y_circle : { 0.0f, 1.0f }) {
    m_vertexes.insert(m_vertexes.end(), { 0.0f, y_circle, 0.0f });

    for (size_t step = 0; step < m_n_corners; step++) {
      float angle = step * angle_step;
      m_vertexes.insert(m_vertexes.end(), {std::cos(angle), y_circle, std::sin(angle)});
    }
  }
}

void Cylinder::_set_indices() {
  // triangles of bottom circle
  unsigned int i_center_bottom = 0;
  for (unsigned int i_vertex = 1; i_vertex <= m_n_corners; ++i_vertex) {
    m_indices.insert(m_indices.end(), { i_center_bottom, i_vertex, i_vertex % m_n_corners + 1 });
  }

  // triangles of top circle
  unsigned int n_vertexes = m_vertexes.size() / 3;
  unsigned int n_vertexes_circle = n_vertexes / 2;
  unsigned int i_center_top = n_vertexes_circle;

  for (unsigned int i_vertex = 1; i_vertex <= m_n_corners; ++i_vertex) {
    unsigned int i_vertex_current = i_vertex + i_center_top;
    unsigned int i_vertex_next = (i_vertex % m_n_corners + 1) + i_center_top;
    m_indices.insert(m_indices.end(), { i_center_top, i_vertex_current, i_vertex_next });
  }

  // pairs of triangles for each side face
  for (unsigned int i_vertex = 1; i_vertex <= m_n_corners; ++i_vertex) {
    m_indices.insert(m_indices.end(), { i_vertex, i_vertex % m_n_corners + 1, i_vertex + i_center_top });
    m_indices.insert(m_indices.end(), { i_vertex % m_n_corners + 1, i_vertex % m_n_corners + i_center_top + 1, i_vertex + i_center_top });
  }
}

unsigned int Cylinder::get_n_elements() const {
  return m_indices.size();
}
