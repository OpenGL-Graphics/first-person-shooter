#ifndef TILEMAP_HPP
#define TILEMAP_HPP

#include <string>
#include <vector>

struct Tilemap {
  // types & enums constants
  using Map = std::vector<std::vector<char>>;
  enum class Tiles : char {
    WALL_H = '-',
    WALL_V = '|',
    DOOR_H = '=',
    DOOR_V = '#',
    SPACE = ' ',

    // interactive characters
    ENEMMY = 'e',

    // props
    TREE = 't',
    WINDOW = 'w',

    // compounds wall composed of two perpendicular walls
    WALL_L = 'L',
    WALL_L_INV = 'I',
  };

  /**
   * `n_rows` includes horiz. wall character at beginning of map,
   * which doesn't change map depth (z-coord)
   */
  unsigned int n_rows;
  unsigned int n_cols;
  Map map;

  Tilemap(const std::string& path);
};

#endif // TILEMAP_HPP
