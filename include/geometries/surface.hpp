#ifndef SURFACE_HPP
#define SURFACE_HPP

#include <geometries/geometry.hpp>

class Surface : public Geometry {
public:
  Surface();
  Surface(const std::vector<float>& vertexes);
  std::vector<float> get_vertexes() const override;
  int get_n_vertexes() const override;
  std::vector<unsigned int> get_indices() const override;

protected:
  std::vector<float> m_vertexes;
  int m_n_vertexes;
  std::vector<unsigned int> m_indices;

private:
  static const std::vector<float> VERTEXES;
  static const std::vector<unsigned int> INDICES;
};

#endif // SURFACE_HPP
