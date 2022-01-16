#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include "geometries/grid.hpp"
#include "geometries/geometry.hpp"
#include "image.hpp"

/**
 * Inspired by triangle-strip terrain from this tutorial:
 * http://www.learnopengles.com/android-lesson-eight-an-introduction-to-index-buffer-objects-ibos/
 */
class Terrain : public Geometry, public Grid {
public:
  Terrain(const Image& heightmap);
  unsigned int get_n_elements() const override;

private:
  /* used to magnify/reduce size of grid */
  const float scale = 1.0f / 100;

  /* heightmap image */
  Image m_image;

  void set_normals();
  void set_positions_from_perlin();
  void set_positions_from_image();
  void set_vertexes_from_paraboloid();
};

#endif // TERRAIN_HPP
