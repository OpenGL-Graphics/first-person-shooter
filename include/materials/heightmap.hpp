#ifndef HEIGHTMAP_HPP
#define HEIGHTMAP_HPP

#include <vector>

#include "perlin/SimplexNoise.h"

/* Heightmap with perlin noise elevations */
struct Heightmap {
  std::vector<std::vector<float>> elevations;

  Heightmap(unsigned int width, unsigned int height);
};

#endif // HEIGHTMAP_HPP
