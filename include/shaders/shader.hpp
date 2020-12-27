// inspired by: https://learnopengl.com/Getting-started/Shaders
#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <string>
#include <iostream>
#include <vector>

class Shader {
  public:
    GLuint m_id;
    GLenum m_type;

    Shader(const std::string& source, GLenum type) {
      // The Cherno: https://www.youtube.com/watch?v=71BLZwRGUJE
      const char* source_shader_char = source.c_str();

      // compile shader
      m_type = type;
      m_id = glCreateShader(type);
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
        std::string type_shader_str = (type == GL_VERTEX_SHADER ? "vertex" : "fragment");
        std::cout << "Shader " << type_shader_str << ": " << message.data() << "\n";

        glDeleteShader(m_id);
        m_id = 0;
      }
    }
};

#endif // SHADER_HPP
