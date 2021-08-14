#include <fstream>
#include <sstream>

#include "utils/file.hpp"

/**
 * Get file content.
 * Used to read shaders files.
 * @param path Path to file to read
 */
std::string File::get_content(const std::string& path) {
  std::ifstream f(path.c_str());
  std::stringstream buffer;
  buffer << f.rdbuf();

  return buffer.str();
}

/**
 * Parse file to get each of its characters into 2D array.
 * Used to get tilemap file content (see `levels/Tilemap`).
 * @param path Path to file to read
 */
std::vector<std::vector<char>> File::get_chars(const std::string& path) {
  std::vector<std::vector<char>> map;
  std::ifstream f(path.c_str());
  std::string line;

  while (std::getline(f, line)) {
    std::vector<char> line_vec(line.begin(), line.end());
    map.push_back(line_vec);
  }

  return map;
}
