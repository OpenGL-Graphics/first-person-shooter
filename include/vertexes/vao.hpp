#ifndef VAO_HPP
#define VAO_HPP

#include <glad/glad.h>
#include <string>
#include <vector>
#include <vertexes/attribute.hpp>

class VAO {
public:
  VAO();
  void free();
  void bind();
  void unbind();
  void set_attribute(const Attribute& attribute);

private:
  GLuint m_id;
};

#endif // VAO_HPP
