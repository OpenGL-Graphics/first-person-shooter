// inspired by: https://learnopengl.com/Getting-started/Shaders
#include <glad/glad.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <shaders/shader.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Program {
  public:
    GLuint m_id;
    GLuint m_id_vertex;
    GLuint m_id_fragment;

    Program(const std::string& path_vertex, const std::string& path_fragment) {
      // read shader source codes into strings (newer GLSL version not supported)
      std::string source_vertex = read_file(path_vertex);
      std::string source_fragment = read_file(path_fragment);

      // create vertex & fragment shaders
      Shader shader_vertex(source_vertex, GL_VERTEX_SHADER);
      Shader shader_fragment(source_fragment, GL_FRAGMENT_SHADER);
      m_id_vertex = shader_vertex.m_id;
      m_id_fragment = shader_fragment.m_id;
      if (m_id_vertex == 0 || m_id_fragment == 0) {
        m_id = 0;
        return;
      }

      // attach shaders to program & link it
      m_id = glCreateProgram();
      glAttachShader(m_id, m_id_vertex);
      glAttachShader(m_id, m_id_fragment);
      glLinkProgram(m_id);
      glValidateProgram(m_id);

      // error handling of program linking
      GLint result_link;
      glGetProgramiv(m_id, GL_LINK_STATUS, &result_link);
      if (result_link == GL_FALSE) {
        GLint length_link;
        glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &length_link);
        std::vector<GLchar> message(length_link);
        glGetProgramInfoLog(m_id, length_link, NULL, message.data());
        std::cout << "program: " << message.data() << "\n";

        glDeleteProgram(m_id);
        m_id = 0;
      }

      // flag attached shaders objects for deletion
      glDeleteShader(m_id_vertex);
      glDeleteShader(m_id_fragment);
    }

    void set_mat4(const std::string& name, const glm::mat4& mat) {
      // set value of uniform variable using its name
      GLuint uniform = glGetUniformLocation(m_id, name.c_str());
      glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(mat));
    }

  private:
    std::string read_file(const std::string& filename) {
      std::ifstream f(filename.c_str());
      std::stringstream buffer;
      buffer << f.rdbuf();

      return buffer.str();
    }
};
