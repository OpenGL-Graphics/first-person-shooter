#ifndef CYLINDER_HPP
#define CYLINDER_HPP

#include <geometries/geometry.hpp>

class Cylinder : public Geometry {
public:
  Cylinder(int n_corners);
  unsigned int get_n_elements() const override;

private:
  int m_n_corners;

  void _set_vertexes();
  void _set_indices();
};

#endif // CYLINDER_HPP
