#ifndef SURFACE_HPP
#define SURFACE_HPP

#include "geometries/geometry.hpp"

class Surface : public Geometry {
public:
  Surface(const glm::vec2& uv_max={1.0f, 1.0f});
  Surface(const std::vector<float>& vertexes);
  std::vector<float> get_vertexes() const override;
  unsigned int get_n_elements() const override;
  std::vector<unsigned int> get_indices() const override;
  std::vector<glm::vec3> get_positions() const override;

private:
  static const std::vector<float> VERTEXES;
  static const std::vector<unsigned int> INDICES;
  static const std::vector<glm::vec3> POSITIONS;

  /* Prevent texture from stretching by pushing uv-coord outside [0, 1] (i.e. repeat) */
  glm::vec2 m_uv_max;
  std::vector<float> _get_vertexes();

  void set_n_elements();
};

#endif // SURFACE_HPP
