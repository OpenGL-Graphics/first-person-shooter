#include "levels/tilemap.hpp"
#include "shader/file.hpp"

Tilemap::Tilemap(const std::string& path) {
  map = File::get_chars(path);
  n_rows = map.size();
  n_cols = map[0].size();
}
