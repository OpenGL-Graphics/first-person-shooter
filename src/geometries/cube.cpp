/**
 * Cubemap texture:
 *   - 6 x 2D images
 *   - Texture coords: 3D direction vector (u, v, w) from cube center to vertexes
 *   - u, v, w: in [0, 1] with point (0, 0) being lower-left corner,
 *              if outside the range, GL_CLAMP_TO_EDGE can clamp coords in [0, 1]
 * https://www.khronos.org/opengl/wiki/Cubemap_Texture
 * https://learnopengl.com/Advanced-OpenGL/Cubemaps
 */
#include "geometries/cube.hpp"

Cube::Cube() {
  // cannot be init in constructor's member initalizer list as they're members of base class
  m_vertexes = VERTEXES;
  m_indices = INDICES;
  m_positions = POSITIONS;

  // duplicated vertexes across triangles
  unsigned int n_faces = 6;
  unsigned int n_triangles = 2 * n_faces;
  m_n_elements = 3 * n_triangles;
}

std::vector<float> Cube::get_vertexes() const {
  return m_vertexes;
}

unsigned int Cube::get_n_elements() const {
  return m_n_elements;
}

std::vector<unsigned int> Cube::get_indices() const {
  return m_indices;
}

std::vector<glm::vec3> Cube::get_positions() const {
  return m_positions;
}

// origin: center of gravity
// coord(x,y,z)        color(r,g,b)      texture(u,v,w)       normal(nx,ny,nz)
const std::vector<float> Cube::VERTEXES = {
  // negative-x (left face)
  -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
  -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
  -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
  -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,

   // positive-x (right face)
   0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
   0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
   0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
   0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,

   // negative-y (bottom face)
  -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
   0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
   0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
  -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,

  // positive-y (top face)
  -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
   0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
   0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
  -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,

  // negative-z (back face)
  -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
   0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
   0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
  -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,

  // positive-z (front face)
  -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
   0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
   0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
  -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
};

const std::vector<unsigned int> Cube::INDICES {
  // negative-x (left face)
  0, 1, 2,
  2, 3, 0,

  // positive-x (right face)
  4, 5, 6,
  6, 7, 4,

  // negative-y (bottom face)
  8, 9, 10,
  10, 11, 8,

  // positive-y (top face)
  12, 13, 14,
  14, 15, 12,

  // negative-z (back face)
  16, 17, 18,
  18, 19, 16,

  // positive-z (front face)
  20, 21, 22,
  22, 23, 20,
};

// copy of vertexes above containing only (x, y, z) coords
const std::vector<glm::vec3> Cube::POSITIONS = {
  // negative-x (left face)
  {-0.5f,  0.5f,  0.5f},
  {-0.5f,  0.5f, -0.5f},
  {-0.5f, -0.5f, -0.5f},
  {-0.5f, -0.5f,  0.5f},

  // positive-x (right face)
  {0.5f,  0.5f,  0.5f},
  {0.5f,  0.5f, -0.5f},
  {0.5f, -0.5f, -0.5f},
  {0.5f, -0.5f,  0.5f},

  // negative-y (bottom face)
  {-0.5f, -0.5f, -0.5f},
  { 0.5f, -0.5f, -0.5f},
  { 0.5f, -0.5f,  0.5f},
  {-0.5f, -0.5f,  0.5f},

  // positive-y (top face)
  {-0.5f,  0.5f, -0.5f},
  { 0.5f,  0.5f, -0.5f},
  { 0.5f,  0.5f,  0.5f},
  {-0.5f,  0.5f,  0.5f},

  // negative-z (back face)
  {-0.5f, -0.5f, -0.5f},
  { 0.5f, -0.5f, -0.5f},
  { 0.5f,  0.5f, -0.5f},
  {-0.5f,  0.5f, -0.5f},

  // positive-z (front face)
  {-0.5f, -0.5f,  0.5f},
  { 0.5f, -0.5f,  0.5f},
  { 0.5f,  0.5f,  0.5f},
  {-0.5f,  0.5f,  0.5f},
};
