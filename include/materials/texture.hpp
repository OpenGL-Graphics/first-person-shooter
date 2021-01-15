#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/glad.h>
#include <string>
#include <vector>

class Texture {
public:
  // two constructors for surfaces & cubes textures
  Texture(const std::string& path, int index=0);
  Texture(const std::vector<std::string>& paths, int index=0);
  virtual void free() final;

protected:

private:
  GLuint m_id;
  GLuint m_type;
  GLenum m_index;

  void generate();
  void from_image(const std::string& path, GLenum type_face=GL_TEXTURE_2D);
  void from_images(const std::vector<std::string>& paths);
};

#endif // TEXTURE_HPP
