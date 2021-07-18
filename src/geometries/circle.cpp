#include <geometries/circle.hpp>
#include <cmath>

Circle::Circle(int n_corners):
  m_n_corners(n_corners)
{
  // calculate vertexes coords on creation
  set_vertexes();
  set_n_elements();
}

void Circle::set_vertexes() {
  float angle = 2*M_PI / m_n_corners;

  for (size_t step = 0; step < m_n_corners; step++) {
    float angle_begin = step * angle;
    float angle_end = (step + 1) * angle;

    // three corners of each slice
    m_vertexes.insert(m_vertexes.end(), {std::cos(angle_begin), 0.0f, std::sin(angle_begin)});
    m_vertexes.insert(m_vertexes.end(), {0.0f, 0.0f, 0.0f});
    m_vertexes.insert(m_vertexes.end(), {std::cos(angle_end), 0.0f, std::sin(angle_end)});
  }
}

void Circle::set_n_elements() {
  m_n_elements = m_n_corners * 3;
}

std::vector<float> Circle::get_vertexes() const {
  return m_vertexes;
}

unsigned int Circle::get_n_elements() const {
  return m_n_elements;
}
