#ifndef TILEMAP_HPP
#define TILEMAP_HPP

#include <string>
#include <vector>

using Map = std::vector<std::vector<char>>;

/*
// dictionary type for heteregenous uniform values
using KeyUniform = std::string;
using ValueUniform = std::variant<glm::mat4, glm::vec3, float, Texture2D, Texture3D>;
using Uniforms = std::unordered_map<KeyUniform, ValueUniform>;
*/

struct Tilemap {
  Map map;

  Tilemap(const std::string& path);
};

#endif // TILEMAP_HPP
