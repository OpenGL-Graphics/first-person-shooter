#include <materials/texture.hpp>
#include <type_traits>
#include <vector>

// template specialization for 2d surface textures
template <>
Texture2D::Texture(const std::string& path, GLenum index):
  m_index(index),
  m_type(GL_TEXTURE_2D)
{
  generate();
  bind();
  configure();
  from_image(path);
  unbind();
}

// template specialization for 3d cubic textures
template <>
Texture3D::Texture(const std::vector<std::string>& paths, GLenum index):
  m_index(index),
  m_type(GL_TEXTURE_CUBE_MAP)
{
  generate();
  bind();
  configure();
  from_images(paths);
  unbind();
}

template <class T>
void Texture<T>::generate() {
  glGenTextures(1, &m_id);
}

template <class T>
void Texture<T>::configure() {
  // wrap texture around mesh
  glTexParameteri(m_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(m_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

template <class T>
void Texture<T>::attach() {
  // attach texture object m_id to texture unit m_index before draw
  glActiveTexture(m_index);
  bind();
}

template <class T>
void Texture<T>::from_image(const std::string& path, GLenum type_face) {
  // define texture from loaded image 
  Image image(path);
  glTexImage2D(type_face, 0, image.format, image.width, image.height, 0, image.format, GL_UNSIGNED_BYTE, image.data);
  m_width = image.width;
  m_height = image.height;
  image.free();
}

template <class T>
void Texture<T>::from_images(const std::vector<std::string>& paths) {
  // 6-sided texture cube using loaded images 
  for (size_t i_texture = 0; i_texture < 6; i_texture++) {
    from_image(paths[i_texture], GL_TEXTURE_CUBE_MAP_POSITIVE_X + i_texture);
  }
}

template <class T>
void Texture<T>::free() {
  glDeleteTextures(1, &m_id);
}

template <class T>
GLenum Texture<T>::get_index() const {
  // used to pass texture index to shaders program
  return m_index - GL_TEXTURE0;
}

template <class T>
int Texture<T>::get_width() const {
  // used to scale HUD
  return m_width;
}

template <class T>
int Texture<T>::get_height() const {
  // used to scale HUD
  return m_height;
}

template <class T>
void Texture<T>::bind() {
  glBindTexture(m_type, m_id);
}

template <class T>
void Texture<T>::unbind() {
  glBindTexture(m_type, 0);
}

// template instantiation to generate class from it (fixes link error)
template class Texture<std::string>;
template class Texture<std::vector<std::string>>;
