#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <glad/glad.h>
#include <string>

struct Image {
  int width;
  int height;
  int n_channels;
  GLenum format;
  unsigned char* data;

  Image(const std::string& path);
  void free();
};

#endif // IMAGE_HPP
