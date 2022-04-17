#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>

#include "program.hpp"
#include "uniforms.hpp"
#include "vertexes/attribute.hpp"
#include "vertexes/vao.hpp"
#include "vertexes/vbo.hpp"
#include "math/transformation.hpp"

struct Renderer {
  /* accessed in derived class `TextRenderer` & in `Player` */
  VBO vbo;

  Renderer(const Program& program, const VBO& vertex_buffer, const std::vector<Attribute>& attributes);
  virtual void draw(const Uniforms& u={}, GLenum mode=GL_TRIANGLES, unsigned int count=0, size_t offset=0) final;
  void draw_with_outlines(const Uniforms& u);
  virtual void free() final;
  void set_transform(const Transformation& t);

private:
  VAO m_vao;
  Program m_program;
  Transformation m_transformation;
};

#endif // RENDERER_HPP
