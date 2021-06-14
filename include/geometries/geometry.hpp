#ifndef MESH_HPP
#define MESH_HPP

#include <vector>

// getters needed for polymorphistic access to class members inside VBO class
class Geometry {
public:
  virtual std::vector<float> get_vertexes() const;
  virtual int get_n_vertexes() const;
  virtual std::vector<unsigned int> get_indices() const;

protected:
  std::vector<float> m_vertexes;
  int m_n_vertexes;
  std::vector<unsigned int> m_indices;
};

#endif // MESH_HPP
