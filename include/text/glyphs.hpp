#ifndef GLYPHS_HPP
#define GLYPHS_HPP

#include <unordered_map>
#include <glm/glm.hpp>

#include "materials/texture.hpp"

// glyph origin at left corner of text baseline
struct Glyph {
  Texture2D texture;
  glm::uvec2 size;
  glm::ivec2 bearing;
  long int advance;
};

// dictionary (key: character & value: texture, its size)
using KeyGlyph = unsigned char;
using ValueGlyph = Glyph;
using Glyphs = std::unordered_map<KeyGlyph, ValueGlyph>;

// characters to extract from font
const unsigned char CHAR_START = ' ';
const unsigned char CHAR_END = 'z';

#endif // GLYPHS_HPP
