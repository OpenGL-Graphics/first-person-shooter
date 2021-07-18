#ifndef VBO_HPP
#define VBO_HPP

#include <glad/glad.h>
#include <geometries/geometry.hpp>

/* Manage VBO & EBO and encapsulate mesh vertexes & their indices */
struct VBO {
  /* needed by `Render` class */
  unsigned int n_elements;
  std::vector<glm::vec3> positions;

  VBO(const Geometry& geometry, bool is_empty=false, GLenum type=GL_STATIC_DRAW);
  void free();
  void bind();
  void unbind();
  void generate();
  void update(const Geometry& geometry);

private:
  GLuint m_id_vbo;
  GLuint m_id_ebo;
  GLenum m_type;

  // polymorphism enabled with virtual methods (in descendents classes of Geometry)
  std::vector<float> m_vertexes;
  std::vector<unsigned int> m_indices;
};

#endif // VBO_HPP
