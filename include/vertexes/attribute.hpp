#ifndef ATTRIBUTE_HPP
#define ATTRIBUTE_HPP

#include <glad/glad.h>
#include <string>
#include <vector>

struct Attribute {
  GLuint id;
  std::string name;
  int size;
  int stride;
  int offset;
};

#endif // ATTRIBUTE_HPP
