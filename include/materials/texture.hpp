#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/glad.h>
#include <string>
#include <vector>
#include <materials/image.hpp>

template <class T>
class Texture {
public:
  Texture(const T&, GLenum index=GL_TEXTURE0);
  void free();
  GLenum get_index() const;
  int get_width() const;
  int get_height() const;

private:
  GLuint m_id;
  GLuint m_type;
  GLenum m_index;
  int m_width;
  int m_height;

  void generate();
  void from_image(const std::string& path, GLenum type_face=GL_TEXTURE_2D);
  void from_images(const std::vector<std::string>& paths);
};

// type of texture for a single image (2d) or a vector (3d)
using Texture2D = Texture<std::string>;
using Texture3D = Texture<std::vector<std::string>>;

#endif // TEXTURE_HPP
