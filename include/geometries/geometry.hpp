#ifndef MESH_HPP
#define MESH_HPP

#include <vector>

// getters needed for polymorphistic access to class members inside VBO class
class Geometry {
public:
  Geometry();
  Geometry(const std::vector<float>& vertexes, const std::vector<unsigned int>& indices);
  virtual std::vector<float> get_vertexes() const;
  virtual int get_n_vertexes() const;
  virtual std::vector<unsigned int> get_indices() const;

protected:
  std::vector<float> m_vertexes;

  /* coordinates for position/normal/texture-coord */
  int m_n_vertexes;

  /* faces (triangles formed by vertexes indexes) */
  std::vector<unsigned int> m_indices;
};

#endif // MESH_HPP
