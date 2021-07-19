#include <geometries/terrain.hpp>
#include <cmath>
#include <iostream>

Terrain::Terrain(unsigned int n_vertexes_x, unsigned int n_vertexes_y):
  m_n_vertexes_x(n_vertexes_x),
  m_n_vertexes_y(n_vertexes_y)
{
  // set_vertexes_from_plan();
  set_vertexes_from_paraboloid();
  set_indices();
  set_n_elements();
  print_indices();
}

/**
 * Set vertexes (positions, colors) from horizontal xy-plan (z = 0)
 * y-axis is the vertical axis in OpenGL (whereas for terrain plan z=0 is the ground)
 */
void Terrain::set_vertexes_from_plan() {
  for (size_t i_vertex_y = 0; i_vertex_y < m_n_vertexes_y; ++i_vertex_y) {
    for (size_t i_vertex_x = 0; i_vertex_x < m_n_vertexes_x; ++i_vertex_x) {
      m_vertexes.insert(m_vertexes.end(), {(float) i_vertex_x, 0.0, (float) i_vertex_y});
      m_vertexes.insert(m_vertexes.end(), {0xff, 0x00, 0x00});
    }
  }
}

/**
 * Set vertexes positions from paraboloid (3d parabola)
 * Generic equation: z = x^2 / a^2 + y^2 / b^2, a & b control curve steepness
 * y-axis is the vertical axis in OpenGL
 * # of vertexes on xy-directions needs to be an odd number, as x (resp. y with height) in [-width/2, width/2]
 * Note: In wireframe mode, flat triangles (degenerates) would still be rendered as a line
 */
void Terrain::set_vertexes_from_paraboloid() {
  // prevents paraboloid from being too steep
  int a = 3;
  int b = 3;

  // can't apply negation to unsigned int in cpp
  int min_x = std::ceil((float) m_n_vertexes_x / -2);
  int max_x = std::ceil((float) m_n_vertexes_x / 2);
  int min_y = std::ceil((float) m_n_vertexes_x / -2);
  int max_y = std::ceil((float) m_n_vertexes_x / 2);

  for (int i_vertex_y = min_y; i_vertex_y < max_y; ++i_vertex_y) {
    for (int i_vertex_x = min_x; i_vertex_x < max_x; ++i_vertex_x) {
      float z = std::pow(i_vertex_x, 2) / std::pow(a, 2) + std::pow(i_vertex_y, 2) / std::pow(b, 2);
      m_vertexes.insert(m_vertexes.end(), {(float) i_vertex_x, z, (float) i_vertex_y});
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

/* Display indices of successive vertexes forming triangle strips (Useful for debugging) */
void Terrain::print_indices() {
  for (unsigned int indice : m_indices) {
    std::cout << indice << ',';
  }
  std::cout << '\n';
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
