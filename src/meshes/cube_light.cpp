#include <meshes/cube_light.hpp>

CubeLight::CubeLight(Program program):
  Cube(program)
{
  // get normal attribute from shader
  set_attribute();
}

void CubeLight::set_attribute() {
  // normal attribute
  GLuint attr_normal = m_program.define_attribute("normal");
  glVertexAttribPointer(attr_normal, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void *) (9 * sizeof(float)));
  glEnableVertexAttribArray(attr_normal);
}
