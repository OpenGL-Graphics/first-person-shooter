#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <glad/glad.h>
#include <string>

/* Wrapper around image pointer returned by `stbi_load()` */
struct Image {
  int width;
  int height;
  GLenum format;
  unsigned char* data;
  std::string path;

  Image(const std::string& p, int desired_channels=0);
  Image(int w=0, int h=0, GLenum f=0, unsigned char* ptr=NULL);
  void free() const;
};

#endif // IMAGE_HPP
