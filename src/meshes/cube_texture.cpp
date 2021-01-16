#include <meshes/cube_texture.hpp>

CubeTexture::CubeTexture(const Program& program, const Texture3D& texture):
  Cube(program),
  m_texture(texture)
{
  // get texture attribute from shaders
  set_attribute();
}

void CubeTexture::draw(GLenum render_type) {
  m_program.set_texture("texture_in", m_texture);
  Mesh::draw(render_type);
}

void CubeTexture::set_attribute() {
  // texture direction attribute
  GLuint attr_texture_dir = m_program.define_attribute("texture_coord");
  glVertexAttribPointer(attr_texture_dir, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void *) (6 * sizeof(float)));
  glEnableVertexAttribArray(attr_texture_dir);
}
