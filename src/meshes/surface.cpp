#include <meshes/surface.hpp>

Surface::Surface(const Program& program):
  Mesh(program)
{
  // initialize vbo buffer
  init_buffers();

  // get attributes from shader
  set_attribute();
}

void Surface::set_attribute() {
  // position attribute (same name as input var in vertex shader)
  GLuint attr_position = m_program.define_attribute("position");
  glVertexAttribPointer(attr_position, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
  glEnableVertexAttribArray(attr_position);

  // texture direction attribute
  GLuint attr_texture_dir = m_program.define_attribute("texture_coord");
  glVertexAttribPointer(attr_texture_dir, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
  glEnableVertexAttribArray(attr_texture_dir);
}

std::vector<float> Surface::get_vertexes() {
  return m_vertexes;
}

int Surface::get_n_vertexes() {
  return m_n_vertexes;
}
