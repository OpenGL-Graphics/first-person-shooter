#ifndef MESH_HPP
#define MESH_HPP

#include <glad/glad.h>
#include <shaders/program.hpp>
#include <shaders/uniforms.hpp>
#include <vector>

// abstract class (note the pure virtual methods)
class Mesh {
public:
  Mesh(const Program& program);
  virtual void draw(const Uniforms& uniforms, GLenum render_type=GL_FILL) final;
  virtual void free() final;

protected:
  std::vector<float> m_vertexes;
  int m_n_vertexes;
  Program m_program;

  virtual void init_buffers() final;
  virtual void set_attribute() = 0;

  // getters needed to access derived classes members here
  virtual std::vector<float> get_vertexes() = 0;
  virtual int get_n_vertexes() = 0;

private:
  GLuint m_vao;
  GLuint m_vbo;
};

#endif // MESH_HPP
