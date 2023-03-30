#include <iostream>

#include "geometries/cylinder.hpp"

using namespace geometry;

/* Calculate vertexes & indices on creation */
Cylinder::Cylinder(int n_corners, float radius, float height):
  // duplicated statring corner for cylinder seam (avoids stretching uv between last & 1st point)
  // https://stackoverflow.com/a/36308796/2228912
  m_n_corners(n_corners + 1),

  m_radius(radius),
  m_height(height)
{
  // reserve space for position & normal coords for every vertex (1: center of circle)
  unsigned int n_vertexes_circle = 1 + m_n_corners;
  unsigned int n_vertexes = 2 * n_vertexes_circle;
  m_vertexes.resize(m_n_coords * n_vertexes);

  // set vertexes xyz & indices
  set_positions();
  set_indices();
  set_normals();
  set_texture_coords();
  set_tangents();

  /*
  for (unsigned int i_indice = 0; i_indice < m_indices.size(); ++i_indice) {
    std::cout << m_indices[i_indice] << " ";
  }
  */
}

void Cylinder::set_positions() {
  // ignore last vertex (i.e. seam)
  float angle_step = 2*M_PI / (m_n_corners - 1);
  unsigned int i_coord = 0;
  const unsigned int stride = m_n_coords - 3;

  for (auto y_circle : { 0.0f, m_height }) {
    // center of top/bottom circle
    m_vertexes[i_coord++] = 0.0f;
    m_vertexes[i_coord++] = y_circle;
    m_vertexes[i_coord++] = 0.0f;
    i_coord += stride;

    // corners of top/bottom circle
    for (size_t step = 0; step < m_n_corners - 1; step++) {
      float angle = step * angle_step;

      m_vertexes[i_coord++] = m_radius * std::cos(angle);
      m_vertexes[i_coord++] = y_circle;
      m_vertexes[i_coord++] = m_radius * std::sin(angle);
      i_coord += stride;
    }

    // seam same as starting corner (corner closing cylinder to avoid stretching uv)
    m_vertexes[i_coord++] = m_radius * std::cos(0.0f);
    m_vertexes[i_coord++] = y_circle;
    m_vertexes[i_coord++] = m_radius * std::sin(0.0f);
    i_coord += stride;
  }
}

/* Same rationale as in `Terrain` (but no triangle strips) */
void Cylinder::set_normals() {
  unsigned int n_vertexes = m_vertexes.size() / m_n_coords;
  std::vector<glm::vec3> normals(n_vertexes, glm::vec3(0.0f));

  // skip triangles of bottom & top circles (hidden) to avoid further smoothing of normals
  size_t i_indice = 0;
  i_indice += 3 * m_n_corners;
  i_indice += 3 * m_n_corners;

  for (; i_indice < m_indices.size() - 2; i_indice += 3) {
    unsigned int i_vertex0 = m_indices[i_indice];
    unsigned int i_vertex1 = m_indices[i_indice + 1];
    unsigned int i_vertex2 = m_indices[i_indice + 2];

    glm::vec3 vertex0 = {m_vertexes[m_n_coords * i_vertex0], m_vertexes[m_n_coords * i_vertex0 + 1], m_vertexes[m_n_coords * i_vertex0 + 2]};
    glm::vec3 vertex1 = {m_vertexes[m_n_coords * i_vertex1], m_vertexes[m_n_coords * i_vertex1 + 1], m_vertexes[m_n_coords * i_vertex1 + 2]};
    glm::vec3 vertex2 = {m_vertexes[m_n_coords * i_vertex2], m_vertexes[m_n_coords * i_vertex2 + 1], m_vertexes[m_n_coords * i_vertex2 + 2]};

    glm::vec3 v0 = vertex1 - vertex0;
    glm::vec3 v1 = vertex2 - vertex1;
    glm::vec3 normal = glm::cross(v0, v1);

    // cross-prod. = zero vector for seam (same start & end point) => normalized to NaN
    if (normal != glm::vec3(0))
      normal = glm::normalize(normal);

    normals[i_vertex0] += normal;
    normals[i_vertex1] += normal;
    normals[i_vertex2] += normal;
  }

  // averaging sum of vectors is equivalent to normalizing it (same dir., diff. mag)
  for (size_t i_vertex = 0; i_vertex < n_vertexes; ++i_vertex) {
    glm::vec3 normal = glm::normalize(normals[i_vertex]);

    // vertex's normal coords inserted after its xyz coords
    m_vertexes[m_n_coords * i_vertex + 3] = normal.x;
    m_vertexes[m_n_coords * i_vertex + 4] = normal.y;
    m_vertexes[m_n_coords * i_vertex + 5] = normal.z;
  }
}

void Cylinder::set_indices() {
  // triangles of bottom circle (vertexes order ensures normal points downwards)
  unsigned int i_center_bottom = 0;
  for (unsigned int i_vertex = 1; i_vertex <= m_n_corners; ++i_vertex) {
    m_indices.insert(m_indices.end(), { i_center_bottom, i_vertex, i_vertex % m_n_corners + 1 });
  }

  // triangles of top circle (vertexes order ensures normal points upwards)
  unsigned int n_vertexes = m_vertexes.size() / m_n_coords;
  unsigned int n_vertexes_circle = n_vertexes / 2;
  unsigned int i_center_top = n_vertexes_circle;

  for (unsigned int i_vertex = 1; i_vertex <= m_n_corners; ++i_vertex) {
    unsigned int i_vertex_current = i_vertex + i_center_top;
    unsigned int i_vertex_next = (i_vertex % m_n_corners + 1) + i_center_top;
    m_indices.insert(m_indices.end(), { i_center_top, i_vertex_next, i_vertex_current });
  }

  // pairs of triangles for each side face (vertexes order ensures normal points outwards)
  for (unsigned int i_vertex = 1; i_vertex <= m_n_corners; ++i_vertex) {
    m_indices.insert(m_indices.end(), { i_vertex % m_n_corners + 1, i_vertex, i_vertex + i_center_top });
    m_indices.insert(m_indices.end(), { i_vertex % m_n_corners + 1, i_vertex + i_center_top, i_vertex % m_n_corners + i_center_top + 1 });
  }
}

/* Repeat uv-texture coords (i.e. uv > 1) to avoid stretching it */
void Cylinder::set_texture_coords() {
  unsigned int i_coord = 0;

  float circumference = 2*M_PI * m_radius;
  float step = circumference / (m_n_corners - 1);

  for (auto v_circle : { 0.0f, m_height }) {
    // center of top/bottom circle (ignored)
    m_vertexes[i_coord + 6] = 0.0f;
    m_vertexes[i_coord + 7] = v_circle;
    i_coord += m_n_coords;

    // top/bottom circle corners
    for (unsigned int i_corner = 0; i_corner < m_n_corners; i_corner++) {
      m_vertexes[i_coord + 6] = i_corner * step;
      m_vertexes[i_coord + 7] = v_circle;
      i_coord += m_n_coords;
    }
  }
}

/* Tangent vector in TBN matrix (for normal mapping) */
void Cylinder::set_tangents() {
  unsigned int n_vertexes = m_vertexes.size() / m_n_coords;

  // tangent from cross-prod between bitangent (y-axis) & normal
  for (size_t i_vertex = 0; i_vertex < n_vertexes; ++i_vertex) {
    glm::vec3 bitangent(0.0f, 1.0f, 0.0f);
    glm::vec3 normal(m_vertexes[m_n_coords * i_vertex + 3], m_vertexes[m_n_coords * i_vertex + 4], m_vertexes[m_n_coords * i_vertex + 5]);
    glm::vec3 tangent = glm::normalize(glm::cross(bitangent, normal));

    // vertex's tangent coords inserted after xyz/normal/uv
    m_vertexes[m_n_coords * i_vertex + 8 ] = tangent.x;
    m_vertexes[m_n_coords * i_vertex + 9 ] = tangent.y;
    m_vertexes[m_n_coords * i_vertex + 10] = tangent.z;
  }
}

unsigned int Cylinder::get_n_elements() const {
  return m_indices.size();
}
