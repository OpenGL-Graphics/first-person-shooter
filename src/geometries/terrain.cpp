#include <cmath>
#include <iostream>

#include "geometries/terrain.hpp"
#include "materials/heightmap.hpp"

Terrain::Terrain(unsigned int n_vertexes_x, unsigned int n_vertexes_y):
  m_n_vertexes_x(n_vertexes_x),
  m_n_vertexes_y(n_vertexes_y)
{
  // reserve space for position & normal coords for every vertex
  m_vertexes.resize(m_n_coords * m_n_vertexes_x * m_n_vertexes_y);

  // set vertexes positions/normals & triangles faces
  set_positions_from_perlin();
  set_indices();
  set_normals();
  set_n_elements();
  // print_indices();
}

/**
 * Set vertexes positions from 2D perlin noise
 * y-axis is the vertical axis in OpenGL (whereas for terrain plan z=0 is the ground)
 */
void Terrain::set_positions_from_perlin() {
  Heightmap heightmap(m_n_vertexes_x, m_n_vertexes_y);
  std::vector<std::vector<float>> elevations =  heightmap.elevations;

  for (size_t i_vertex_y = 0; i_vertex_y < m_n_vertexes_y; ++i_vertex_y) {
    for (size_t i_vertex_x = 0; i_vertex_x < m_n_vertexes_x; ++i_vertex_x) {
      float z = elevations[i_vertex_y][i_vertex_x] * 10.0f;
      unsigned int i_vertex = i_vertex_y * m_n_vertexes_x + i_vertex_x;

      // vertex's xyz position
      m_vertexes[m_n_coords * i_vertex] = i_vertex_x;
      m_vertexes[m_n_coords * i_vertex + 1] = z;
      m_vertexes[m_n_coords * i_vertex + 2] = i_vertex_y;
    }
  }
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
 * Calculate normal for each triangle from the cross-product of its two edges
 * then consider vertex's normal the average of normals of triangles that have this vertex as a corner
 */
void Terrain::set_normals() {
  // print_indices();
  unsigned int n_vertexes = m_n_vertexes_x * m_n_vertexes_y;
  std::vector<glm::vec3> normals(n_vertexes, glm::vec3(0.0f));
  std::vector<unsigned int> n_adjacent_triangles(n_vertexes, 0);

  // loop over all triangles
  for (size_t i_indice = 0; i_indice < m_indices.size() - 2; ++i_indice) {
    unsigned int i_vertex0 = m_indices[i_indice];
    unsigned int i_vertex1 = m_indices[i_indice + 1];
    unsigned int i_vertex2 = m_indices[i_indice + 2];

    // skip degenerate triangles
    if (i_vertex0 == i_vertex1 || i_vertex1 == i_vertex2) {
      continue;
    }

    glm::vec3 vertex0 = {m_vertexes[m_n_coords * i_vertex0], m_vertexes[m_n_coords * i_vertex0 + 1], m_vertexes[m_n_coords * i_vertex0 + 2]};
    glm::vec3 vertex1 = {m_vertexes[m_n_coords * i_vertex1], m_vertexes[m_n_coords * i_vertex1 + 1], m_vertexes[m_n_coords * i_vertex1 + 2]};
    glm::vec3 vertex2 = {m_vertexes[m_n_coords * i_vertex2], m_vertexes[m_n_coords * i_vertex2 + 1], m_vertexes[m_n_coords * i_vertex2 + 2]};

    // calculate triangle's normal (invert normal for even-positionned triangles)
    glm::vec3 v0 = vertex1 - vertex0;
    glm::vec3 v1 = vertex2 - vertex0;
    glm::vec3 normal = (i_indice % 2 == 0 ? glm::cross(v0, v1) : glm::cross(v1, v0));

    // add up triangles's normals & assign to each vertex in triangle
    normal = glm::normalize(normal);
    normals[i_vertex0] += normal;
    normals[i_vertex1] += normal;
    normals[i_vertex2] += normal;
    ++n_adjacent_triangles[i_vertex0];
    ++n_adjacent_triangles[i_vertex1];
    ++n_adjacent_triangles[i_vertex2];
  }

  // average out sum of normals accord. to # of triangles adjacent to vertex & set its normal
  for (size_t i_vertex = 0; i_vertex < n_vertexes; ++i_vertex) {
    glm::vec3 normal = glm::normalize(normals[i_vertex] / (float) n_adjacent_triangles[i_vertex]);

    // vertex's normal coords inserted after its xyz coords
    m_vertexes[m_n_coords * i_vertex + 3] = normal.x;
    m_vertexes[m_n_coords * i_vertex + 4] = normal.y;
    m_vertexes[m_n_coords * i_vertex + 5] = normal.z;
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
