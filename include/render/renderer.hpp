#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>

#include "shaders/program.hpp"
#include "shaders/uniforms.hpp"
#include "vertexes/attribute.hpp"
#include "vertexes/vao.hpp"
#include "vertexes/vbo.hpp"
#include "physics/bounding_box.hpp"

struct Renderer {
  BoundingBox bounding_box;

  Renderer(const Program& program, const VBO& vbo, const std::vector<Attribute>& attributes);
  virtual void draw(Uniforms& uniforms) final;
  virtual void free() final;
  void set_transform(const glm::mat4& mat_model);
  void move(const glm::vec3& offset);

protected:
  // accessed in derived class `TextRenderer`
  VBO m_vbo;

private:
  VAO m_vao;
  Program m_program;
  glm::mat4 m_mat_model;
};

#endif // RENDERER_HPP
