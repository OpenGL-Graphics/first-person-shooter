#include <meshes/cube_light.hpp>

CubeLight::CubeLight(const Program& program):
  Cube(program)
{
  // get normal attribute from shader
  set_attribute(program);
}

void CubeLight::set_attribute(const Program& program) {
  // normal attribute
  GLuint attr_normal = program.define_attribute("normal");
  glVertexAttribPointer(attr_normal, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void *) (9 * sizeof(float)));
  glEnableVertexAttribArray(attr_normal);
}
