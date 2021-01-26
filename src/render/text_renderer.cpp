#include <render/text_renderer.hpp>
#include <geometries/surface.hpp>
#include <iostream>

TextRenderer::TextRenderer(const Program& program, const VBO& vbo, const std::vector<Attribute>& attributes, const Font& font):
  m_glyphs(font.extract_glyphs()),
  Renderer(program, vbo, attributes)
{
}

void TextRenderer::draw(Uniforms& uniforms, const std::string& text) {
  // inspired by https://learnopengl.com/In-Practice/Text-Rendering
  float x = 0;

  for (const unsigned char& c : text) {
    // get glyph texture corresponding to character
    Glyph glyph(m_glyphs.at(c));
    uniforms["texture2d"] = glyph.texture;
    float width = glyph.size.x;
    float height = glyph.size.y;
    float bearing_x = glyph.bearing.x;
    float bearing_y = glyph.bearing.y;

    // Use dimensions & bearing to create glyph's bounding box
    // Origin is lower-left corner for texture & upper-left corner for bitmap
    float x_prime = x + bearing_x;
    const std::vector<float> vertexes = {
      x_prime,         bearing_y,             0.0f, 0.0f,
      x_prime + width, bearing_y,             1.0f, 0.0f,
      x_prime + width, -(height - bearing_y), 1.0f, 1.0f,
      x_prime + width, -(height - bearing_y), 1.0f, 1.0f,
      x_prime,         -(height - bearing_y), 0.0f, 1.0f,
      x_prime,         bearing_y,             0.0f, 0.0f,
    };
    m_vbo.update(Surface(vertexes));

    // render character & advance to following one
    Renderer::draw(uniforms);
    x += glyph.advance;
  }
}

Glyphs TextRenderer::get_glyphs() const {
  // used to free glyphs textures
  return m_glyphs;
}
