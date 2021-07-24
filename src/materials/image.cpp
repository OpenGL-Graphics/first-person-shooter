#include <iostream>

#include "materials/image.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Image::Image(const std::string& p):
  path(p)
{
 // load image using its path (opengl origin at lower-left corner of image)
  stbi_set_flip_vertically_on_load(true);
  int n_channels;
  data = stbi_load(path.c_str(), &width, &height, &n_channels, 0);

  if (data == nullptr) {
    std::cout << "Image " << path << " doesn't exist" << std::endl;
  }

  switch (n_channels) {
    case 1:
      format = GL_RED;
      break;
    case 3:
      format = GL_RGB;
      break;
    case 4:
      format = GL_RGBA;
      break;
  }
}

Image::Image(int w, int h, GLenum f, unsigned char* ptr):
  width(w),
  height(h),
  format(f),
  data(ptr)
{
  // used to load glyph bitmap for a font into image &
  // as default constructor (TextRenderer::m_glyphs contains textures that need to be init)
}

void Image::free() const {
  // glyph bitmap don't have a path (avoid double free, as freed auto by freetype)
  if (!path.empty()) {
    stbi_image_free(data);
  }
}
