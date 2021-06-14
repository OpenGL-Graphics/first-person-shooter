#include <materials/texture.hpp>

/*
 * template specialization for 2d surface textures
 */
template <>
void Texture2D::from_image() {
  // 2d texture from given image (save width & height for HUD scaling)
  m_width = m_image.width;
  m_height = m_image.height;
  glTexImage2D(GL_TEXTURE_2D, 0, m_image.format, m_width, m_height, 0, m_image.format, GL_UNSIGNED_BYTE, m_image.data);
}

template <>
Texture2D::Texture(const Image& image, GLenum index):
  m_image(image),
  m_type(GL_TEXTURE_2D),
  m_index(index)
{
  // default constructor needed to init class member TextRenderer::m_glyphs
  if (image.data != NULL) {
    generate();
    bind();
    configure();
    from_image();
    unbind();
  }
}

template <>
int Texture2D::get_width() const {
  // used to scale HUD surface
  return m_width;
}

template <>
int Texture2D::get_height() const {
  // used to scale HUD surface
  return m_height;
}

template <>
void Texture2D::free() {
  // delete image associated & texture
  m_image.free();
  glDeleteTextures(1, &m_id);
}

/*
 * template specialization for 3d cubic textures
 */
template <>
void Texture3D::from_images() {
  // 6-sided texture cube using given images 
  for (size_t i_texture = 0; i_texture < m_image.size(); i_texture++) {
    Image image(m_image[i_texture]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i_texture, 0, image.format, image.width, image.height, 0, image.format, GL_UNSIGNED_BYTE, image.data);
  }
}

template <>
Texture3D::Texture(const std::vector<Image>& image, GLenum index):
  m_image(image),
  m_type(GL_TEXTURE_CUBE_MAP),
  m_index(index)
{
  generate();
  bind();
  configure();
  from_images();
  unbind();
}

template <>
void Texture3D::free() {
  for (const Image& image : m_image) {
    image.free();
  }

  glDeleteTextures(1, &m_id);
}

/*
 * generic methods
 */
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
void Texture<T>::bind() {
  glBindTexture(m_type, m_id);
}

template <class T>
void Texture<T>::unbind() {
  glBindTexture(m_type, 0);
}

template <class T>
void Texture<T>::attach() {
  // attach texture object m_id to texture unit m_index before draw
  glActiveTexture(m_index);
  bind();
}

template <class T>
GLenum Texture<T>::get_index() const {
  // used to pass texture index to shaders program
  return m_index - GL_TEXTURE0;
}

// template instantiation to generate class from it (fixes link error)
template class Texture<Image>;
template class Texture<std::vector<Image>>;
