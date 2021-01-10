#ifndef CUBE_LIGHT_HPP
#define CUBE_LIGHT_HPP

#include <meshes/cube.hpp>

class CubeLight : public Cube {
public:
  CubeLight(Program program):
    Cube(program)
  {
    // get normal attribute from shader
    set_attribute();
  }

protected:
  void set_attribute() override {
    // normal attribute
    GLuint attr_normal = m_program.define_attribute("normal");
    glVertexAttribPointer(attr_normal, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void *) (9 * sizeof(float)));
    glEnableVertexAttribArray(attr_normal);
  }
};

#endif // CUBE_LIGHT_HPP
