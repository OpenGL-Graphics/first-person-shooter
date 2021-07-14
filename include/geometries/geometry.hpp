#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <vector>
#include <glm/glm.hpp>

/* Virtual getters needed for polymorphistic call to methods of derived classes inside VBO class */
class Geometry {
public:
  Geometry();
  Geometry(const std::vector<float>& vertexes, const std::vector<unsigned int>& indices, const std::vector<glm::vec3>& positions);
  virtual std::vector<float> get_vertexes() const;
  virtual int get_n_vertexes() const;
  virtual std::vector<unsigned int> get_indices() const;
  virtual std::vector<glm::vec3> get_positions() const;

protected:
  std::vector<float> m_vertexes;

  /* coordinates for position/normal/texture-coord */
  int m_n_vertexes;

  /* faces (triangles formed by vertexes indexes) */
  std::vector<unsigned int> m_indices;

  /* used to calculate bounding box */
  std::vector<glm::vec3> m_positions;
};

#endif // GEOMETRY_HPP
