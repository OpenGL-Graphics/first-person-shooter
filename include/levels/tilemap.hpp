#ifndef TILEMAP_HPP
#define TILEMAP_HPP

#include <string>
#include <vector>

#include "materials/texture.hpp"

struct Tilemap {
  // types & enums constants
  using Map = std::vector<std::vector<char>>;
  enum class Tiles : char {
    WALL_H = '_',
    WALL_V = '|',
    DOOR_H = '=',
    DOOR_V = '#',
    SPACE = ' ',
  };

  unsigned int n_rows;
  unsigned int n_cols;
  Map map;

  Tilemap(const std::string& path);
};

#endif // TILEMAP_HPP
