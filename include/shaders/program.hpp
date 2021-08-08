// inspired by: https://learnopengl.com/Getting-started/Shaders
#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

#include "shaders/uniforms.hpp"

class Program {
  public:
    Program(const std::string& path_vertex, const std::string& path_fragment);
    void set_mat4(const std::string& name, const glm::mat4& mat);
    void set_vec3(const std::string& name, const glm::vec3& vect);
    void set_float(const std::string& name, float value);
    void set_int(const std::string& name, int value);
    void set_uniforms(const Uniforms& uniforms);
    void use();
    void unuse();
    void free();
    bool has_failed();
    GLuint define_attribute(const std::string& attribute) const;

  private:
    GLuint m_id;
};

#endif // PROGRAM_HPP
