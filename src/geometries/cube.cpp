#include <geometries/cube.hpp>

Cube::Cube():
  m_vertexes(VERTEXES),
  m_n_vertexes(6 * 2 * 3),
  m_indices(INDICES)
{
}

std::vector<float> Cube::get_vertexes() const {
  return m_vertexes;
}

int Cube::get_n_vertexes() const {
  return m_n_vertexes;
}

std::vector<unsigned int> Cube::get_indices() const {
  return m_indices;
}

// origin: center of gravity
// coord(x,y,z)        color(r,g,b)      texture(u,v,w)       normal(nx,ny,nz)
const std::vector<float> Cube::VERTEXES = {
  // negative-x (left face)
  -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
  -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
  -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
  // -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
  -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
  // -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,

   // positive-x (right face)
   0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
   0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
   0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
   // 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
   0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
   // 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,

   // negative-y (bottom face)
  -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
   0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
   0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
   // 0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
  -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
  // -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,

  // positive-y (top face)
  -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
   0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
   0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
  //  0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
  -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
  // -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,

  // negative-z (back face)
  -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
   0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
   0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
  // 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
  -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
  // -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,

  // positive-z (front face)
  -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
   0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
   0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
  // 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
  -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
  // -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f 
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
