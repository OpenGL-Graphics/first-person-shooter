#include <geometries/geometry.hpp>

/* default constructor needed by classes inheriting from Geometry */
Geometry::Geometry() {
}

/* generic geometry used to render assimp meshes */
Geometry::Geometry(const std::vector<float>& vertexes, const std::vector<unsigned int>& indices):
  m_vertexes(vertexes),
  m_indices(indices)
{
}

std::vector<float> Geometry::get_vertexes() const {
  return m_vertexes;
}

int Geometry::get_n_vertexes() const {
  return m_vertexes.size();
}

std::vector<unsigned int> Geometry::get_indices() const {
  return m_indices;
}
