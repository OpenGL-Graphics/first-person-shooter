#include <geometries/surface.hpp>

Surface::Surface() {
  // cannot be init in constructor's member initalizer list as they're members of base class
  m_vertexes = VERTEXES;
  m_n_vertexes = 6;
  m_indices = INDICES;
  m_positions = POSITIONS;
}

Surface::Surface(const std::vector<float>& vertexes) {
  // init its vertexes with glyph dimensions
  m_vertexes = vertexes;
  m_n_vertexes = 6;
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

std::vector<glm::vec3> Surface::get_positions() const {
  return m_positions;
}

//coord(x,y) texture(u,v)
const std::vector<float> Surface::VERTEXES {
  // origin: lower-left corner for opengl textures
  0.0f, 0.0f, 0.0f, 0.0f,
  1.0f, 0.0f, 1.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
};

const std::vector<unsigned int> Surface::INDICES {
  0, 1, 2, // triangle 1
  2, 3, 0, // triangle 2
};

// copy of vertexes above containing only (x, y) coords (z-coord = 0)
const std::vector<glm::vec3> Surface::POSITIONS {
  {0.0f, 0.0f, 0.0f},
  {1.0f, 0.0f, 0.0f},
  {1.0f, 1.0f, 0.0f},
  {0.0f, 1.0f, 0.0f},
};
