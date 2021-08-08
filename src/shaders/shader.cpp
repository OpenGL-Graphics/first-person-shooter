#include <vector>
#include <iostream>

#include "shaders/shader.hpp"

Shader::Shader(const std::string& source, GLenum type) {
  // The Cherno: https://www.youtube.com/watch?v=71BLZwRGUJE
  const char* source_shader_char = source.c_str();

  // compile shader
  m_type = type;
  m_id = glCreateShader(m_type);
  glShaderSource(m_id, 1, &source_shader_char, NULL);
  glCompileShader(m_id);

  // error handling for shader compilation
  GLint result;
  glGetShaderiv(m_id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    GLint length;
    glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &length);
    std::vector<GLchar> message(length);
    glGetShaderInfoLog(m_id, length, NULL, message.data());
    std::string type_shader_str = (m_type == GL_VERTEX_SHADER ? "vertex" : "fragment");
    std::cout << "Shader " << type_shader_str << ": " << message.data() << "\n";

    free();
    m_id = 0;
  }
}

void Shader::free() {
  glDeleteShader(m_id);
}
