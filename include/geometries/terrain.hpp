#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include "geometries/geometry.hpp"

/**
 * Inspired by triangle-strip terrain from this tutorial:
 * http://www.learnopengles.com/android-lesson-eight-an-introduction-to-index-buffer-objects-ibos/
 */
class Terrain : public Geometry {
public:
  Terrain(unsigned int n_vertexes_x, unsigned int n_vertexes_y);
  unsigned int get_n_elements() const override;

private:
  /* # of vertexes in horizontal & vertical directions */
  unsigned int m_n_vertexes_x;
  unsigned int m_n_vertexes_y;

  /* # of coords by vertex (xyz & normal) */
  const unsigned int m_n_coords = 6;

  void set_n_elements();
  void set_indices();
  void set_normals();
  void set_positions_from_perlin();
  void set_vertexes_from_plan();
  void set_vertexes_from_paraboloid();
  void print_indices();
};

#endif // TERRAIN_HPP
