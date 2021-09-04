#ifndef TEXT_RENDERER_HPP
#define TEXT_RENDERER_HPP

#include "render/renderer.hpp"
#include "text/font.hpp"
#include "text/glyphs.hpp"

class TextRenderer : public Renderer {
public:
  TextRenderer(const Program& program, const VBO& vbo, const std::vector<Attribute>& attributes, const Font& font);
  void draw_text(const std::string& text, const Uniforms& u={});
  Glyphs get_glyphs() const;

private:
  Glyphs m_glyphs;
};

#endif // TEXT_RENDERER_HPP
