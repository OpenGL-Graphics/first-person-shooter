#include <geometries/text.hpp>

Text::Text(const Program& program, const std::string& text, float x, float y):
  m_program(program),
  m_text(text),
  m_x(x),
  m_y(y)
{
}

void Text::draw() {
}

void Text::free() {
}
