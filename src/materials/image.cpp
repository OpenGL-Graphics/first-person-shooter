#include <materials/image.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

Image::Image(const std::string& path) {
  // load image from its path
  stbi_set_flip_vertically_on_load(true);
  data = stbi_load(path.c_str(), &width, &height, &n_channels, 0);

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

void Image::free() {
  stbi_image_free(data);
}
