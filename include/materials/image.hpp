#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <glad/glad.h>
#include <string>

struct Image {
  int width;
  int height;
  GLenum format;
  unsigned char* data;
  std::string path;

  Image(const std::string& p);
  Image(int w=0, int h=0, GLenum f=0, unsigned char* ptr=NULL);
  void free() const;
};

#endif // IMAGE_HPP
