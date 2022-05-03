#ifndef WALL_ENTRY_HPP
#define WALL_ENTRY_HPP

#include <glm/glm.hpp>

#include "entries/wall_orientation.hpp"

struct WallEntry {
  /* position tile (offset calculate later accord. to orientation) */
  glm::vec3 position;

  WallOrientation orientation;
};

#endif // WALL_ENTRY_HPP
