#ifndef FONT_HPP
#define FONT_HPP

#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "text/glyphs.hpp"

class Font {
public:
  Font(const std::string& path);
  Glyphs extract_glyphs() const;

private:
  std::string m_path;
  FT_Library m_ft;
  FT_Face m_face;

  void load_library();
  void load_face();
  FT_Bitmap char_to_bitmap(unsigned char c) const;
  void free() const;
};

#endif // FONT_HPP
