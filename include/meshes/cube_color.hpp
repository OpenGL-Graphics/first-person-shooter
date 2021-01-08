#ifndef CUBE_COLOR_HPP
#define CUBE_COLOR_HPP

#include <meshes/cube.hpp>

class CubeColor : public Cube {
public:
  CubeColor(Program program):
    Cube(program)
  {
    // get color attribute from shader
    set_attribute();
  }

protected:
  void set_attribute() override {
    // color attribute
    GLuint attr_color = m_program.define_attribute("color");
    glVertexAttribPointer(attr_color, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(attr_color);
  }
};

#endif // CUBE_COLOR_HPP
