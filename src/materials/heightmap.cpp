#include "materials/heightmap.hpp"
#include "perlin/SimplexNoise.h"

/* Generate 2D heightmap of the given dimensions with Perlin noise elevations */
Heightmap::Heightmap(unsigned int width, unsigned int height) {
  // perlin noise parameters from https://github.com/SRombauts/SimplexNoiseCImg
  unsigned int octaves = 10;
  float frequency = 1 / 10.0f;
  float amplitude = 1.0f;
  float lacunarity = 2.0f;
  float persistence = 0.5f;

  SimplexNoise simplex_noise(frequency, amplitude, persistence);
  elevations.resize(height);

  for (size_t i_row = 0; i_row < height; ++i_row) {
    elevations[i_row].resize(width);

    for (size_t i_col = 0; i_col < width; ++i_col) {
      elevations[i_row][i_col] = simplex_noise.fractal(octaves, i_col, i_row);
    }
  }
}
