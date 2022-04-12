#include "geometries/surface.hpp"

/**
 * @param size Needed to avoid stretching wall texture (i.e. uv-coords follow xyz coords)
 */
Surface::Surface(const glm::vec2& size):
  m_size(size)
{
  // cannot be init in constructor's member initalizer list as they're members of base class
  m_vertexes = _get_vertexes();
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

/**
 * xy only for position as z-coord set to 0 in vertex shader
 * origin: lower-left corner for opengl textures
 * Vertexes sorted in CCW order (i.e. defines visible face) in `Surface::INDICES` => normal sticking out of front-face (i.e. z-axis)
 */
std::vector<float> Surface::_get_vertexes() {
  return {
    //coord(x,y)        texture(u,v)        normal(nx,ny,nz)
    0.0f,     0.0f,     0.0f,     0.0f,     0.0f, 0.0f, 1.0f,
    m_size.x, 0.0f,     m_size.x, 0.0f,     0.0f, 0.0f, 1.0f,
    m_size.x, m_size.y, m_size.x, m_size.y, 0.0f, 0.0f, 1.0f,
    0.0f,     m_size.y, 0.0f,     m_size.y, 0.0f, 0.0f, 1.0f,
  };
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
