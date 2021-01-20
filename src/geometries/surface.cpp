#include <geometries/surface.hpp>

Surface::Surface():
  m_vertexes(VERTEXES),
  m_n_vertexes(6)
{
}

std::vector<float> Surface::get_vertexes() const {
  return m_vertexes;
}

int Surface::get_n_vertexes() const {
  return m_n_vertexes;
}

//coord(x,y) texture(u,v)
const std::vector<float> Surface::VERTEXES {
  // origin: lower-left corner
  0.0f, 0.0f, 0.0f, 0.0f,
  1.0f, 0.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 0.0f, 0.0f,
};
