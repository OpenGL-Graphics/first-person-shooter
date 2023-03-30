#include <iostream>

#include "render/text_renderer.hpp"
#include "geometries/surface.hpp"

using namespace geometry;

TextRenderer::TextRenderer(const Program& program, const std::vector<Attribute>& attributes, const Font& font):
  Renderer(program, Surface(), attributes, true),
  m_glyphs(font.extract_glyphs())
{
}

void TextRenderer::draw_text(const std::string& text, const Uniforms& u) {
  // inspired by https://learnopengl.com/In-Practice/Text-Rendering
  float x = 0;
  Uniforms uniforms = u;

  for (const char& c : text) {
    // get glyph texture corresponding to character
    Glyph glyph(m_glyphs.at(c));
    uniforms["texture2d"] = glyph.texture;
    float width = glyph.size.x;
    float height = glyph.size.y;
    float bearing_x = glyph.bearing.x;
    float bearing_y = glyph.bearing.y;

    /**
     * Face triangles (see also graph in: https://learnopengl.com/In-Practice/Text-Rendering)
     *  _u
     * |
     * v
     *     0  3
     * y   o--o
     * |_x |\ |
     *     | \|
     *     o--o
     *     1  2
     */

    // Use dimensions & bearing to create glyph's bounding box (xy origin at character baseline's left point)
    // upper-left corner is character bitmap's origin (=> uv-coords origin at upper-left & v-axis inverted) - no flip-v with stb_image
    // TODO: text is only updated on event, but draw_text() is called in each frame => don't recalculate geometry :/
    float x_prime = x + bearing_x;
    const std::vector<float> vertexes = {
      //coord(x,y)                            normal(nx,ny,nz)  texture(u,v)
      x_prime,         bearing_y,             0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // point (0)
      x_prime,         -(height - bearing_y), 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // point (1)
      x_prime + width, -(height - bearing_y), 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // point (2)
      x_prime + width, bearing_y,             0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // point (3)
    };
    // use EBO to pass vertexes indices
    vbo.update(Surface(vertexes));

    // render character & advance to following one
    Renderer::draw(uniforms);
    x += glyph.advance;
  }
}

Glyphs TextRenderer::get_glyphs() const {
  // used to free glyphs textures
  return m_glyphs;
}
