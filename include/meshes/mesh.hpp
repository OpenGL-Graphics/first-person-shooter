#ifndef MESH_HPP
# define MESH_HPP

#include <glad/glad.h>
#include <shaders/program.hpp>
#include <vector>

// abstract class (note the pure virtual set_attributes() method)
class Mesh {
public:
  Mesh(const std::vector<float>& vertexes, int n_vertexes);
  virtual void draw() final;
  virtual void free();

protected:
  virtual void set_attribute(const Program& program) = 0;

private:
  GLuint m_vao;
  GLuint m_vbo;
  int m_n_vertexes;

  void init_buffers(const std::vector<float>& vertexes);
};

#endif // MESH_HPP
