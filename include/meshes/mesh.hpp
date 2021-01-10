#ifndef MESH_HPP
# define MESH_HPP

#include <glad/glad.h>
#include <shaders/program.hpp>
#include <vector>

// abstract class (note the pure virtual set_attributes() method)
class Mesh {
public:
  Mesh(Program program, const std::vector<float>& vertexes);
  virtual void draw() final;
  virtual void free();

protected:
  Program m_program;

  virtual void set_attribute() = 0;

private:
  GLuint m_vao;
  GLuint m_vbo;

  void init_buffers(const std::vector<float>& vertexes);
};

#endif // MESH_HPP
