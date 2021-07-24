#include "geometries/geometry.hpp"

/* default constructor needed by classes inheriting from Geometry */
Geometry::Geometry() {
}

/* generic geometry used to render assimp meshes */
Geometry::Geometry(const std::vector<float>& vertexes, const std::vector<unsigned int>& indices, const std::vector<glm::vec3>& positions):
  m_vertexes(vertexes),
  m_indices(indices),
  m_positions(positions)
{
}

std::vector<float> Geometry::get_vertexes() const {
  return m_vertexes;
}

unsigned int Geometry::get_n_elements() const {
  return m_vertexes.size();
}

std::vector<unsigned int> Geometry::get_indices() const {
  return m_indices;
}

std::vector<glm::vec3> Geometry::get_positions() const {
  return m_positions;
}
