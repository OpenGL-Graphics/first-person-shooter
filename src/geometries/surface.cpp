#include <geometries/surface.hpp>

Surface::Surface() {
  // cannot be init in constructor's member initalizer list as they're members of base class
  m_vertexes = VERTEXES;
  m_indices = INDICES;
  m_positions = POSITIONS;
  set_n_elements();
}

/* Init its vertexes with glyph dimensions */
Surface::Surface(const std::vector<float>& vertexes) {
  m_vertexes = vertexes;
  set_n_elements();
}

/* Vertexes duplicated between two triangles */
void Surface::set_n_elements() {
  unsigned int n_triangles = 2;
  m_n_elements = 3 * n_triangles;
}

std::vector<float> Surface::get_vertexes() const {
  return m_vertexes;
}

unsigned int Surface::get_n_elements() const {
  return m_n_elements;
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
