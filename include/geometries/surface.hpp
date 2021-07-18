#ifndef SURFACE_HPP
#define SURFACE_HPP

#include <geometries/geometry.hpp>

class Surface : public Geometry {
public:
  Surface();
  Surface(const std::vector<float>& vertexes);
  std::vector<float> get_vertexes() const override;
  unsigned int get_n_elements() const override;
  std::vector<unsigned int> get_indices() const override;
  std::vector<glm::vec3> get_positions() const override;

private:
  static const std::vector<float> VERTEXES;
  static const std::vector<unsigned int> INDICES;
  static const std::vector<glm::vec3> POSITIONS;

  void set_n_elements();
};

#endif // SURFACE_HPP
