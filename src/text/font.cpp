#include <text/font.hpp>
#include <iostream>

Font::Font(const std::string& path):
  m_path(path)
{
  // load freetype library & given font
  load_library();
  load_face();
}

Glyphs Font::extract_glyphs() const {
  Glyphs glyphs;

  // textures pixels aligned with image rows (by default expects image width % 4 = 0)
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  for (unsigned char c = CHAR_START; c <= CHAR_END; c++) {
    // grayscale bitmap from font character
    FT_Bitmap bitmap(char_to_bitmap(c));
    Image image(bitmap.width, bitmap.rows, GL_RED, bitmap.buffer);

    // insert texture for glyph's bitmap into map
    glyphs.insert(std::pair<KeyGlyph, ValueGlyph>(c, {
      .texture = Texture2D(image),
      .size = glm::uvec2(bitmap.width, bitmap.rows),
      .bearing = glm::ivec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top),
      .advance = m_face->glyph->advance.x >> 6,
    }));
  }

  // reset texture pixels alignment to default
  // glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

  // free freetype library & font
  free();

  return glyphs;
}

void Font::load_library() {
  // initialize freetype library
  if (FT_Init_FreeType(&m_ft)) {
    std::cout << "Failed to initialize FreeType" << std::endl;
  }
}

void Font::load_face() {
  // load font
  if (FT_New_Face(m_ft, m_path.c_str(), 0, &m_face)) {
    std::cout << "Failed to load font" << std::endl;
  }

  // set font pixel size
  FT_Set_Pixel_Sizes(m_face, 0, 48);
}

FT_Bitmap Font::char_to_bitmap(unsigned char c) const {
  // bitmap automatically freed by function below
  if (FT_Load_Char(m_face, c, FT_LOAD_RENDER)) {
    std::cout << "Failed to load glyph" << std::endl;
  }

  return m_face->glyph->bitmap;
}

void Font::free() const {
  // discard font & library (FT frees glyph bitmap automatically)
  FT_Done_Face(m_face);
  FT_Done_FreeType(m_ft);
}
