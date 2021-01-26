#ifndef VBO_HPP
#define VBO_HPP

#include <glad/glad.h>
#include <geometries/geometry.hpp>

// manage VBO and encapsulate mesh vertexes
class VBO {
public:
  VBO(const Geometry& geometry, bool is_empty=false, GLenum type=GL_STATIC_DRAW);
  void free();
  void bind();
  void unbind();
  std::vector<float> get_vertexes() const;
  int get_n_vertexes() const;
  void update(const Geometry& geometry);

private:
  GLuint m_id;
  GLenum m_type;

  // polymorphism enabled with virtual methods (not with volatile ref. class members)
  std::vector<float> m_vertexes;
  int m_n_vertexes;
};

#endif // VBO_HPP
