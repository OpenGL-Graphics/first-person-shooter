#include <geometries/cylinder.hpp>
#include <iostream>
#include <cmath>

Cylinder::Cylinder(int n_corners):
  m_n_corners(n_corners)
{
  // calculate vertexes coords on creation
  set_vertexes();
  set_n_elements();
}

void Cylinder::set_vertexes() {
  // three corners of each slice, in each circle
  float angle = 2*M_PI / m_n_corners;

  for (auto i_circle : {0.0f, 1.0f}) {
    for (size_t step = 0; step < m_n_corners; step++) {
      float angle_begin = step * angle;
      float angle_end = (step + 1) * angle;

      m_vertexes.insert(m_vertexes.end(), {std::cos(angle_begin), i_circle, std::sin(angle_begin)});
      m_vertexes.insert(m_vertexes.end(), {0.0f, i_circle, 0.0f});
      m_vertexes.insert(m_vertexes.end(), {std::cos(angle_end), i_circle, std::sin(angle_end)});
    }
  }

  // link bottom and top circles vertexes using 2 triangles/face (skip center of circle)
  int n_elements = m_vertexes.size();
  int n_elements_circle = n_elements / 2;
  for (size_t i_vertex = 0; i_vertex < n_elements_circle; i_vertex += 9) {
    m_vertexes.insert(m_vertexes.end(), {m_vertexes[i_vertex], m_vertexes[i_vertex + 1], m_vertexes[i_vertex + 2]});
    m_vertexes.insert(m_vertexes.end(), {m_vertexes[(i_vertex + 6)], m_vertexes[(i_vertex + 6) + 1], m_vertexes[(i_vertex + 6) + 2]});
    m_vertexes.insert(m_vertexes.end(), {m_vertexes[(i_vertex+n_elements_circle)], m_vertexes[(i_vertex+n_elements_circle) + 1], m_vertexes[(i_vertex+n_elements_circle) + 2]});

    m_vertexes.insert(m_vertexes.end(), {m_vertexes[(i_vertex+n_elements_circle)], m_vertexes[(i_vertex+n_elements_circle) + 1], m_vertexes[(i_vertex+n_elements_circle) + 2]});
    m_vertexes.insert(m_vertexes.end(), {m_vertexes[(i_vertex+n_elements_circle + 6)], m_vertexes[(i_vertex+n_elements_circle + 6) + 1], m_vertexes[(i_vertex+n_elements_circle + 6) + 2]});
    m_vertexes.insert(m_vertexes.end(), {m_vertexes[(i_vertex + 6)], m_vertexes[(i_vertex + 6) + 1], m_vertexes[(i_vertex + 6) + 2]});
  }
}

void Cylinder::set_n_elements() {
  int n_vertexes_circles = 2 * (m_n_corners * 3);
  int n_vertexes_sides = m_n_corners * 2 * 3;
  m_n_elements = n_vertexes_circles + n_vertexes_sides;
}


std::vector<float> Cylinder::get_vertexes() const {
  return m_vertexes;
}

unsigned int Cylinder::get_n_elements() const {
  return m_n_elements;
}
