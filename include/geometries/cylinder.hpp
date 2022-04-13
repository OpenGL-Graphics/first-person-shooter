#ifndef CYLINDER_HPP
#define CYLINDER_HPP

#include <geometries/geometry.hpp>

class Cylinder : public Geometry {
public:
  Cylinder(int n_corners);
  unsigned int get_n_elements() const override;

private:
  /* # of coords by vertex (xyz & normal & texture coords) */
  const unsigned int m_n_coords = 6;

  int m_n_corners;

  void set_positions();
  void set_normals();
  void set_indices();
};

#endif // CYLINDER_HPP
