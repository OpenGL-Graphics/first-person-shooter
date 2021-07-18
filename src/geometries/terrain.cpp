#include <geometries/terrain.hpp>

Terrain::Terrain(unsigned int n_vertexes_x, unsigned int n_vertexes_y):
  m_n_vertexes_x(n_vertexes_x),
  m_n_vertexes_y(n_vertexes_y)
{
  set_vertexes();
  set_indices();
  set_n_elements();
}

/**
 * Calculate vertexes (positions, colors)
 * y-axis is vertical axis in OpenGL (whereas for terrain plan z=0 is the ground)
 */
void Terrain::set_vertexes() {
  for (size_t i_vertex_y = 0; i_vertex_y < m_n_vertexes_y; ++i_vertex_y) {
    for (size_t i_vertex_x = 0; i_vertex_x < m_n_vertexes_x; ++i_vertex_x) {
      m_vertexes.insert(m_vertexes.end(), {(float) i_vertex_x, 0.0, (float) i_vertex_y});
      m_vertexes.insert(m_vertexes.end(), {0xff, 0x00, 0x00});
    }
  }
}

/**
 * Calculate vertexes indices used for triangle strips
 * http://www.learnopengles.com/android-lesson-eight-an-introduction-to-index-buffer-objects-ibos/
 */
void Terrain::set_indices() {
  for (size_t i_vertex_y = 0; i_vertex_y < m_n_vertexes_y - 1; ++i_vertex_y) {
    for (size_t i_vertex_x = 0; i_vertex_x < m_n_vertexes_x; ++i_vertex_x) {
      // successive triangles overlap (advance by 1 vertex each time)
      m_indices.push_back(i_vertex_x + i_vertex_y * m_n_vertexes_x);
      m_indices.push_back(m_n_vertexes_x + i_vertex_x + i_vertex_y * m_n_vertexes_x);
    }

    // degenerate triangles (area = 0) by repeating some vertexes to go to next row
    if (i_vertex_y != m_n_vertexes_y - 2) {
      m_indices.push_back((i_vertex_y + 2) * m_n_vertexes_x - 1);
      m_indices.push_back((i_vertex_y + 1) * m_n_vertexes_x);
    }
  }
}

/**
 * Number of elements to draw (i.e. # of indices in triangle strip mode)
 * https://stackoverflow.com/a/14842779/2228912
 */
void Terrain::set_n_elements() {
  m_n_elements = m_indices.size();
}

/* Needed by `VBO()` otherwise would call `Geometry::get_n_vertexes()` */
unsigned int Terrain::get_n_elements() const {
  return m_n_elements;
}
