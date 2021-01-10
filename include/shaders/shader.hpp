// inspired by: https://learnopengl.com/Getting-started/Shaders
#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <string>

class Shader {
public:
  GLuint m_id;

  Shader(const std::string& source, GLenum type);
  void free();

private:
  GLenum m_type;
};

#endif // SHADER_HPP
