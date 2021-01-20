#include <geometries/pyramid.hpp>

std::vector<float> Pyramid::get_vertexes() const {
  return m_vertexes;
}

int Pyramid::get_n_vertexes() const {
  return m_n_vertexes;
}
