#ifndef PLANE_HPP
#define PLANE_HPP

#include "geometries/geometry.hpp"
#include "geometries/grid.hpp"
#include "image.hpp"

/**
 * Inspired by triangle-strip terrain from this tutorial:
 * http://www.learnopengles.com/android-lesson-eight-an-introduction-to-index-buffer-objects-ibos/
 */
class Plane : public Geometry, public Grid {
public:
  Plane(unsigned int n_vertexes_x, unsigned int n_vertexes_y);
  unsigned int get_n_elements() const override;

private:
  /* used to magnify/reduce size of grid */
  const float scale = 1.0f / 50;

  void set_normals();
  void set_positions_from_grid();
  void set_vertexes_from_paraboloid();
};

#endif // PLANE_HPP
