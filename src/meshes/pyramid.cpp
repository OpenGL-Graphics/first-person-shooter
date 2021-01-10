#include <meshes/pyramid.hpp>

Pyramid::Pyramid(const Program& program):
  Mesh(VERTEXES, N_VERTEXES)
{
  // get position attribute from shader
  set_attribute(program);
}

void Pyramid::set_attribute(const Program& program) {
  // position attribute
  GLuint attr_position = program.define_attribute("position");
  glVertexAttribPointer(attr_position, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
  glEnableVertexAttribArray(attr_position);

  // normal attribute
  GLuint attr_normal = program.define_attribute("normal");
  glVertexAttribPointer(attr_normal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
  glEnableVertexAttribArray(attr_normal);
}

// coord(x,y,z)       normal(nx,ny,nz)
const std::vector<float> Pyramid::VERTEXES = {
  // front-right face
   0.0f, 0.0f,  0.5f,  1.0f,  0.0f, 1.0f,
   0.5f, 0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
   0.0f, 0.5f,  0.0f,  1.0f,  0.0f, 1.0f,

  // front-left face
   0.0f, 0.0f,  0.5f, -1.0f,  0.0f, 1.0f,
  -0.5f, 0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
   0.0f, 0.5f,  0.0f, -1.0f,  0.0f, 1.0f,

  // back-right face
   0.5f, 0.0f,  0.0f,  1.0f,  0.0f, -1.0f,
   0.0f, 0.0f, -0.5f,  1.0f,  0.0f, -1.0f,
   0.0f, 0.5f,  0.0f,  1.0f,  0.0f, -1.0f,

  // back-left face
   0.0f, 0.0f, -0.5f, -1.0f,  0.0f, -1.0f,
  -0.5f, 0.0f,  0.0f, -1.0f,  0.0f, -1.0f,
   0.0f, 0.5f,  0.0f, -1.0f,  0.0f, -1.0f,

  // bottom face
   0.5f, 0.0f,  0.0f,  0.0f, -1.0f, 0.0f,
   0.0f, 0.0f,  0.5f,  0.0f, -1.0f, 0.0f,
  -0.5f, 0.0f,  0.0f,  0.0f, -1.0f, 0.0f,
   0.5f, 0.0f,  0.0f,  0.0f, -1.0f, 0.0f,
   0.0f, 0.0f, -0.5f,  0.0f, -1.0f, 0.0f,
  -0.5f, 0.0f,  0.0f,  0.0f, -1.0f, 0.0f,
};
