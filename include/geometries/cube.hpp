#ifndef CUBE_HPP
#define CUBE_HPP

#include <geometries/geometry.hpp>

class Cube : public Geometry {
public:
  Cube();
  std::vector<float> get_vertexes() const override;
  int get_n_vertexes() const override;
  std::vector<unsigned int> get_indices() const override;

protected:
  std::vector<float> m_vertexes;
  int m_n_vertexes;
  std::vector<unsigned int> m_indices;

private:
  static const std::vector<unsigned int> INDICES;
  static const std::vector<float> VERTEXES;
};

#endif // CUBE_HPP
