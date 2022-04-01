#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <geometries/geometry.hpp>

class Sphere : public Geometry {
public:
  Sphere(unsigned int n_longitudes, unsigned int n_latitudes);
  std::vector<float> get_vertexes() const override;
  unsigned int get_n_elements() const override;

private:
  unsigned int m_n_longitudes;
  unsigned int m_n_latitudes;

  /* # of coords by vertex (xyz & normal) */
  const unsigned int m_n_coords = 6;

  /* needed to fill xyz & normals coords */
  unsigned int m_n_vertexes;

  void set_vertexes();
  void set_indices();
  void set_normals();
};

#endif // SPHERE_HPP
