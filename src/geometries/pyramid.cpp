#include "geometries/pyramid.hpp"

using namespace geometry;

std::vector<float> Pyramid::get_vertexes() const {
  return m_vertexes;
}

int Pyramid::get_n_vertexes() const {
  return m_n_vertexes;
}
