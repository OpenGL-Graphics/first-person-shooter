#include <materials/texture.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

Texture::Texture(const std::string& path, int index):
  m_index(GL_TEXTURE0 + index),
  m_type(GL_TEXTURE_2D)
{
  generate();
  from_image(path);
}

Texture::Texture(const std::vector<std::string>& paths, int index):
  m_index(GL_TEXTURE0 + index),
  m_type(GL_TEXTURE_CUBE_MAP)
{
  generate();
  from_images(paths);
}

void Texture::generate() {
  // generate & bind texture
  glGenTextures(1, &m_id);
  glActiveTexture(m_index);
  glBindTexture(m_type, m_id);

  // wrap texture around mesh
  glTexParameteri(m_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(m_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::from_image(const std::string& path, GLenum type_face) {
  // load image from its path
  int width, height, n_channels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* image = stbi_load(path.c_str(), &width, &height, &n_channels, 0);

  GLenum format;
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

  // define texture from loaded image 
  glTexImage2D(type_face, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
  stbi_image_free(image);
}

void Texture::from_images(const std::vector<std::string>& paths) {
  // 6-sided texture cube using loaded images 
  for (size_t i_texture = 0; i_texture < 6; i_texture++) {
    from_image(paths[i_texture], GL_TEXTURE_CUBE_MAP_POSITIVE_X + i_texture);
  }
}

void Texture::free() {
  glDeleteTextures(1, &m_id);
}
