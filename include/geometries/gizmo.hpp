#ifndef GIZMO_HPP
#define GIZMO_HPP

#include "geometries/geometry.hpp"

/* xyz axes drawn at origin */
class Gizmo : public Geometry {
public:
  Gizmo();

private:
  void set_vertexes();
  void set_indices();
  void set_n_elements();
};

#endif // GIZMO_HPP
