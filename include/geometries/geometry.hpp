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
  virtual std::vector<unsigned int> get_indices() const;
  virtual std::vector<glm::vec3> get_positions() const;

  /**
   * # of elements to render by `glDrawElements()` (= # of vertexes, except for triangle strips = # of indices)
   * Used by `VBO` (and Renderer)
   */
  virtual unsigned int get_n_elements() const;

// vertexes & indices set in derived classes
protected:
  /* coordinates for position/normal/texture-coord */
  std::vector<float> m_vertexes;

  /* # of elements to render */
  int m_n_elements;

  /* faces (triangles formed by vertexes indexes) */
  std::vector<unsigned int> m_indices;

  /* used to calculate bounding box */
  std::vector<glm::vec3> m_positions;
};

#endif // GEOMETRY_HPP
