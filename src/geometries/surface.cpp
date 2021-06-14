#include <geometries/surface.hpp>

Surface::Surface():
  m_vertexes(VERTEXES),
  m_n_vertexes(6),
  m_indices(INDICES)
{
}

Surface::Surface(const std::vector<float>& vertexes):
  m_vertexes(vertexes),
  m_n_vertexes(6)
{
  // init its vertexes with glyph dimensions
}

std::vector<float> Surface::get_vertexes() const {
  return m_vertexes;
}

int Surface::get_n_vertexes() const {
  return m_n_vertexes;
}

std::vector<unsigned int> Surface::get_indices() const {
  return m_indices;
}

//coord(x,y) texture(u,v)
const std::vector<float> Surface::VERTEXES {
  // origin: lower-left corner for opengl textures
  0.0f, 0.0f, 0.0f, 0.0f,
  1.0f, 0.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  // 1.0f, 1.0f, 1.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  // 0.0f, 0.0f, 0.0f, 0.0f,
};

const std::vector<unsigned int> Surface::INDICES {
  0, 1, 2, // triangle 1
  2, 3, 0, // triangle 2
};
