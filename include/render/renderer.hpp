#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>

#include "shaders/program.hpp"
#include "shaders/uniforms.hpp"
#include "vertexes/attribute.hpp"
#include "vertexes/vao.hpp"
#include "vertexes/vbo.hpp"

class Renderer {
public:
  Renderer(const Program& program, const VBO& vbo, const std::vector<Attribute>& attributes);
  virtual void draw(const Uniforms& uniforms) final;
  virtual void free() final;

protected:
  VAO m_vao;
  VBO m_vbo;
  Program m_program;
};

#endif // RENDERER_HPP
