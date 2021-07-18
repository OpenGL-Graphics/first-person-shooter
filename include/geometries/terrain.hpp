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

private:
  /* # of vertexes in horizontal & vertical directions */
  unsigned int m_n_vertexes_x;
  unsigned int m_n_vertexes_y;

  void set_vertexes();
  void set_indices();
};

#endif // TERRAIN_HPP
