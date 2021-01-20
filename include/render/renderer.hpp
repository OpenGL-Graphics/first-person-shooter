#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <shaders/program.hpp>
#include <shaders/uniforms.hpp>
#include <vector>
#include <vertexes/attribute.hpp>
#include <vertexes/vao.hpp>
#include <vertexes/vbo.hpp>

class Renderer {
public:
  Renderer(const Program& program, const VBO& vbo, const std::vector<Attribute>& attributes);
  void draw(const Uniforms& uniforms);
  virtual void free() final;

private:
  VAO m_vao;
  VBO m_vbo;
  Program m_program;
};

#endif // RENDERER_HPP
