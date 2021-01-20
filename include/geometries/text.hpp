// inspired by https://github.com/rougier/freetype-gl/blob/master/demos/subpixel.c
#ifndef TEXT_HPP
#define TEXT_HPP

#include <shaders/program.hpp>
#include <vector>

class Text {
public:
  Text(const Program& program, const std::string& text, float x, float y);
  void draw();
  void free();

private:
  Program m_program;
  std::string m_text;
  float m_x;
  float m_y;

  void apply_texture();
};

#endif // TEXT_HPP
