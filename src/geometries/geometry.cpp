#include <geometries/geometry.hpp>

// definition of non-pure virtual methods (avoid vtable linking error)
std::vector<float> Geometry::get_vertexes() const {
  return m_vertexes;
}

int Geometry::get_n_vertexes() const {
  return m_n_vertexes;
}
