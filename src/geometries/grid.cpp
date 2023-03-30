#include <iostream>

#include "geometries/grid.hpp"

using namespace geometry;

Grid::Grid(unsigned int n_vertexes_x, unsigned int n_vertexes_y):
  m_n_vertexes_x(n_vertexes_x),
  m_n_vertexes_y(n_vertexes_y)
{
}

/**
 * Set vertexes 2D texture coordinates
 * Texture coords in [0, 1] with (0, 0) being lower-left corner of texture image
 */
void Grid::set_texture_coords(std::vector<float>& vertexes) {
  for (size_t i_vertex_y = 0; i_vertex_y < m_n_vertexes_y; ++i_vertex_y) {
    for (size_t i_vertex_x = 0; i_vertex_x < m_n_vertexes_x; ++i_vertex_x) {
      unsigned int i_vertex = i_vertex_y * m_n_vertexes_x + i_vertex_x;
      float coord_x = (float) i_vertex_x / (m_n_vertexes_x - 1);
      float coord_y = (float) i_vertex_y / (m_n_vertexes_y - 1);

      // vertex's texture coords (skip coords for xyz & normal)
      vertexes[m_n_coords * i_vertex + 6] = coord_x;
      vertexes[m_n_coords * i_vertex + 7] = coord_y;
    }
  }
}

/**
 * Calculate vertexes indices used for triangle strips
 * http://www.learnopengles.com/android-lesson-eight-an-introduction-to-index-buffer-objects-ibos/
 * Triangle strips require less # of vertices indices compared to GL_TRIANGLES in RAM: https://en.wikipedia.org/wiki/Triangle_strip
 * @returns indices
 */
void Grid::set_indices(std::vector<unsigned int>& indices) {
  for (size_t i_vertex_y = 0; i_vertex_y < m_n_vertexes_y - 1; ++i_vertex_y) {
    for (size_t i_vertex_x = 0; i_vertex_x < m_n_vertexes_x; ++i_vertex_x) {
      // successive triangles overlap (advance by 1 vertex each time)
      indices.push_back(i_vertex_x + i_vertex_y * m_n_vertexes_x);
      indices.push_back(m_n_vertexes_x + i_vertex_x + i_vertex_y * m_n_vertexes_x);
    }

    // degenerate triangles (area = 0) by repeating some vertexes to go to next row
    if (i_vertex_y != m_n_vertexes_y - 2) {
      indices.push_back((i_vertex_y + 2) * m_n_vertexes_x - 1);
      indices.push_back((i_vertex_y + 1) * m_n_vertexes_x);
    }
  }
}

/* Display indices of successive vertexes forming triangle strips (Useful for debugging) */
void Grid::print_indices(std::vector<unsigned int>& indices) {
  for (unsigned int indice : indices) {
    std::cout << indice << ',';
  }
  std::cout << '\n';
}
