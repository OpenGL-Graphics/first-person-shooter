#ifndef CUBE_HPP
#define CUBE_HPP

#include "geometries/geometry.hpp"

class Cube : public Geometry {
public:
  Cube();
  std::vector<float> get_vertexes() const override;
  int get_n_vertexes() const override;
  std::vector<unsigned int> get_indices() const override;
  std::vector<glm::vec3> get_positions() const override;

private:
  static const std::vector<unsigned int> INDICES;
  static const std::vector<float> VERTEXES;
  static const std::vector<glm::vec3> POSITIONS;
};

#endif // CUBE_HPP
