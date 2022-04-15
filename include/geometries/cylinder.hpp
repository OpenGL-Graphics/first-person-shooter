#ifndef CYLINDER_HPP
#define CYLINDER_HPP

#include <geometries/geometry.hpp>

class Cylinder : public Geometry {
public:
  Cylinder(int n_corners, float radius=1.0f, float height=1.0f);
  unsigned int get_n_elements() const override;

private:
  /* # of coords by vertex (xyz & normal & texture coords & tangent) */
  const unsigned int m_n_coords = 11;

  int m_n_corners;
  float m_radius;
  float m_height;

  void set_positions();
  void set_normals();
  void set_indices();
  void set_texture_coords();
  void set_tangents();
};

#endif // CYLINDER_HPP
