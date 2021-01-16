#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/glad.h>
#include <string>
#include <vector>

template <class T>
class Texture {
public:
  Texture(const T&, GLenum index=GL_TEXTURE0);
  void free();
  int get_index() const;

private:
  GLuint m_id;
  GLuint m_type;
  GLenum m_index;

  void generate();
  void from_image(const std::string& path, GLenum type_face=GL_TEXTURE_2D);
  void from_images(const std::vector<std::string>& paths);
};

// type of texture for a single image (2d) or a vector (3d)
typedef Texture<std::string> Texture2D;
typedef Texture<std::vector<std::string>> Texture3D;

#endif // TEXTURE_HPP
