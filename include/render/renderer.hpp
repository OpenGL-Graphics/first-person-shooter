#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>

#include "shaders/program.hpp"
#include "shaders/uniforms.hpp"
#include "vertexes/attribute.hpp"
#include "vertexes/vao.hpp"
#include "vertexes/vbo.hpp"
#include "physics/bounding_box.hpp"
#include "math/transformation.hpp"

struct Renderer {
  BoundingBox bounding_box;
  /* Used to translate level's surfaces (i.e. tiles) */
  glm::mat4 model_mat;

  /* Used in `LevelRenderer` */
  Transformation transformation;

  Renderer(const Program& program, const VBO& vbo, const std::vector<Attribute>& attributes);
  virtual void draw(const Uniforms& u={}, GLenum mode=GL_TRIANGLES) final;
  void draw_with_outlines(const Uniforms& u);
  virtual void free() final;
  void set_transform(const Transformation& t);
  void move(const glm::vec3& offset);

protected:
  // accessed in derived class `TextRenderer`
  VBO m_vbo;

private:
  VAO m_vao;
  Program m_program;
};

#endif // RENDERER_HPP
