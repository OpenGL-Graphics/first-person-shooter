#include <meshes/cylinder.hpp>
#include <iostream>

Cylinder::Cylinder(const Program& program, int n_corners):
  Mesh(program),
  m_n_corners(n_corners)
{
  // initialize vbo buffer
  init_buffers();

  // get position attribute from shader
  set_attribute();
}

void Cylinder::set_attribute() {
  // position attribute
  GLuint attr_position = m_program.define_attribute("position");
  glVertexAttribPointer(attr_position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
  glEnableVertexAttribArray(attr_position);
}

std::vector<float> Cylinder::get_vertexes() {
  // three corners of each slice, in each circle
  float angle = 2*M_PI / m_n_corners;

  for (auto i_circle : {0.0f, 1.0f}) {
    for (size_t step = 0; step < m_n_corners; step++) {
      float angle_begin = step * angle;
      float angle_end = (step + 1) * angle;

      m_vertexes.insert(m_vertexes.end(), {std::cos(angle_begin), i_circle, std::sin(angle_begin)});
      m_vertexes.insert(m_vertexes.end(), {0.0f, i_circle, 0.0f});
      m_vertexes.insert(m_vertexes.end(), {std::cos(angle_end), i_circle, std::sin(angle_end)});
    }
  }

  // link bottom and top circles vertexes using 2 triangles/face (skip center of circle)
  int n_elements = m_vertexes.size();
  int n_elements_circle = n_elements / 2;
  for (size_t i_vertex = 0; i_vertex < n_elements_circle; i_vertex += 9) {
    m_vertexes.insert(m_vertexes.end(), {m_vertexes[i_vertex], m_vertexes[i_vertex + 1], m_vertexes[i_vertex + 2]});
    m_vertexes.insert(m_vertexes.end(), {m_vertexes[(i_vertex + 6)], m_vertexes[(i_vertex + 6) + 1], m_vertexes[(i_vertex + 6) + 2]});
    m_vertexes.insert(m_vertexes.end(), {m_vertexes[(i_vertex+n_elements_circle)], m_vertexes[(i_vertex+n_elements_circle) + 1], m_vertexes[(i_vertex+n_elements_circle) + 2]});

    m_vertexes.insert(m_vertexes.end(), {m_vertexes[(i_vertex+n_elements_circle)], m_vertexes[(i_vertex+n_elements_circle) + 1], m_vertexes[(i_vertex+n_elements_circle) + 2]});
    m_vertexes.insert(m_vertexes.end(), {m_vertexes[(i_vertex+n_elements_circle + 6)], m_vertexes[(i_vertex+n_elements_circle + 6) + 1], m_vertexes[(i_vertex+n_elements_circle + 6) + 2]});
    m_vertexes.insert(m_vertexes.end(), {m_vertexes[(i_vertex + 6)], m_vertexes[(i_vertex + 6) + 1], m_vertexes[(i_vertex + 6) + 2]});
  }

  return m_vertexes;
}

int Cylinder::get_n_vertexes() {
  int n_vertexes_circles = 2 * (m_n_corners * 3);
  int n_vertexes_sides = m_n_corners * 2 * 3;
  m_n_vertexes = n_vertexes_circles + n_vertexes_sides;

  return m_n_vertexes;
}
