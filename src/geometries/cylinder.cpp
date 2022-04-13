#include "geometries/cylinder.hpp"

/* Calculate vertexes & indices on creation */
Cylinder::Cylinder(int n_corners):
  m_n_corners(n_corners)
{
  // reserve space for position & normal coords for every vertex (1: center of circle)
  unsigned int n_vertexes_circle = 1 + n_corners;
  unsigned int n_vertexes = 2 * n_vertexes_circle;
  m_vertexes.resize(m_n_coords * n_vertexes);

  // set vertexes xyz & indices
  set_positions();
  set_indices();
  set_normals();
}

void Cylinder::set_positions() {
  // center of bottom circle & its corners (then same for top circle)
  float angle_step = 2*M_PI / m_n_corners;
  unsigned int i_vertex = 0;

  for (auto y_circle : { 0.0f, 1.0f }) {
    m_vertexes[i_vertex++] = 0.0f;
    m_vertexes[i_vertex++] = y_circle;
    m_vertexes[i_vertex++] = 0.0f;
    i_vertex += 3;

    for (size_t step = 0; step < m_n_corners; step++) {
      float angle = step * angle_step;

      m_vertexes[i_vertex++] = std::cos(angle);
      m_vertexes[i_vertex++] = y_circle;
      m_vertexes[i_vertex++] = std::sin(angle);
      i_vertex += 3;
    }
  }
}

/* Same rationale as in `Terrain` (but no triangle strips) */
void Cylinder::set_normals() {
  unsigned int n_vertexes = m_vertexes.size() / m_n_coords;
  std::vector<glm::vec3> normals(n_vertexes, glm::vec3(0.0f));

  for (size_t i_indice = 0; i_indice < m_indices.size() - 2; i_indice += 3) {
    unsigned int i_vertex0 = m_indices[i_indice];
    unsigned int i_vertex1 = m_indices[i_indice + 1];
    unsigned int i_vertex2 = m_indices[i_indice + 2];

    glm::vec3 vertex0 = {m_vertexes[m_n_coords * i_vertex0], m_vertexes[m_n_coords * i_vertex0 + 1], m_vertexes[m_n_coords * i_vertex0 + 2]};
    glm::vec3 vertex1 = {m_vertexes[m_n_coords * i_vertex1], m_vertexes[m_n_coords * i_vertex1 + 1], m_vertexes[m_n_coords * i_vertex1 + 2]};
    glm::vec3 vertex2 = {m_vertexes[m_n_coords * i_vertex2], m_vertexes[m_n_coords * i_vertex2 + 1], m_vertexes[m_n_coords * i_vertex2 + 2]};

    glm::vec3 v0 = vertex1 - vertex0;
    glm::vec3 v1 = vertex2 - vertex1;
    glm::vec3 normal = glm::normalize(glm::cross(v0, v1));

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

unsigned int Cylinder::get_n_elements() const {
  return m_indices.size();
}
