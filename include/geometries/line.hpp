#ifndef LINE_HPP
#define LINE_HPP

#include <geometries/geometry.hpp>

/**
 * Line between two given points
 * No need to override virtual methods from `Geometry` class,
 * as they only return field members, declared in base class (e.g. `Geometry::m_vertexes`), set in current constructor
 */
class Line : public Geometry {
public:
  Line(const glm::vec3& point_start, const glm::vec3& point_end);

private:
  glm::vec3 m_point_start;
  glm::vec3 m_point_end;

  void set_vertexes();
  void set_indices();
  void set_n_elements();
};

#endif // LINE_HPP
