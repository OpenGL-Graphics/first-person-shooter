#ifndef VBO_HPP
#define VBO_HPP

#include <glad/glad.h>
#include <geometries/geometry.hpp>

// manage VBO & EBO and encapsulate mesh vertexes & their indices
class VBO {
public:
  VBO(const Geometry& geometry, bool is_empty=false, GLenum type=GL_STATIC_DRAW);
  void free();
  void bind();
  void unbind();
  void generate();
  int get_n_vertexes() const;
  void update(const Geometry& geometry);

private:
  GLuint m_id_vbo;
  GLuint m_id_ebo;
  GLenum m_type;

  // polymorphism enabled with virtual methods (in descendents classes of Geometry)
  std::vector<float> m_vertexes;
  int m_n_vertexes;
  std::vector<unsigned int> m_indices;
};

#endif // VBO_HPP
