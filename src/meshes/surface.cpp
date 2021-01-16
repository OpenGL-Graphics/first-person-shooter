#include <meshes/surface.hpp>

Surface::Surface(const Program& program, const Texture2D& texture):
  Mesh(program),
  m_texture(texture)
{
  // initialize vbo buffer
  init_buffers();

  // get attributes from shader
  set_attribute();
}

void Surface::draw(GLenum render_type) {
  m_program.set_texture("texture_in", m_texture);
  Mesh::draw(render_type);
}

void Surface::set_attribute() {
  // position attribute (same name as input var in vertex shader)
  GLuint attr_position = m_program.define_attribute("position");
  glVertexAttribPointer(attr_position, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
  glEnableVertexAttribArray(attr_position);

  // texture direction attribute
  GLuint attr_texture_dir = m_program.define_attribute("texture_coord");
  glVertexAttribPointer(attr_texture_dir, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
  glEnableVertexAttribArray(attr_texture_dir);
}

std::vector<float> Surface::get_vertexes() {
  return m_vertexes;
}

int Surface::get_n_vertexes() {
  return m_n_vertexes;
}
