#include <cmath>
#include <iostream>
#include <algorithm>

#include "geometries/plane.hpp"

/* Construct random terrain using Perlin noise values */
Plane::Plane(unsigned int n_vertexes_x, unsigned int n_vertexes_y):
  Grid(n_vertexes_x, n_vertexes_y)
{
  // reserve space for position & normal coords for every vertex
  m_vertexes.resize(m_n_coords * m_n_vertexes_x * m_n_vertexes_y);

  // set vertexes positions/normals & triangles faces
  set_positions_from_grid();
  set_indices(m_indices);
  set_normals();
  set_texture_coords(m_vertexes);
  // print_indices();
}

/**
 * Set vertexes positions from flat horizontal xy-plan (y = 0)
 */
void Plane::set_positions_from_grid() {
  for (size_t i_row = 0; i_row < m_n_vertexes_y; ++i_row) {
    for (size_t i_col = 0; i_col < m_n_vertexes_x; ++i_col) {
      unsigned int i_vertex = i_row * m_n_vertexes_x + i_col;

      m_vertexes[m_n_coords * i_vertex] = i_col * scale;  // miniature sized
      m_vertexes[m_n_coords * i_vertex + 1] = 0.0f;
      m_vertexes[m_n_coords * i_vertex + 2] = i_row * scale;
    }
  }
}

/**
 * Set normal for all vertexes to vertical y-axis
 */
void Plane::set_normals() {
  for (size_t i_vertex = 0; i_vertex < m_n_vertexes_x * m_n_vertexes_y; ++i_vertex) {
    // vertex's normal coords inserted after its xyz coords
    m_vertexes[m_n_coords * i_vertex + 3] = 0.0f;
    m_vertexes[m_n_coords * i_vertex + 4] = 1.0f;
    m_vertexes[m_n_coords * i_vertex + 5] = 0.0f;
  }
}

/**
 * Needed by `VBO()` otherwise would call `Geometry::get_n_vertexes()`
 * Number of elements to draw (i.e. # of indices in triangle strip mode)
 * https://stackoverflow.com/a/14842779/2228912
 */
unsigned int Plane::get_n_elements() const {
  return m_indices.size();
}
