#include <meshes/circle.hpp>
#include <iostream>

Circle::Circle(const Program& program, int n_corners):
  m_n_corners(n_corners)
{
  // initialize vbo buffer
  init_buffers();

  // get position attribute from shader
  set_attribute(program);
}

void Circle::set_attribute(const Program& program) {
  // position attribute
  GLuint attr_position = program.define_attribute("position");
  glVertexAttribPointer(attr_position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
  glEnableVertexAttribArray(attr_position);
}

std::vector<float> Circle::get_vertexes() {
  float angle = 2*M_PI / m_n_corners;
  std::cout << "angle: " << angle * (180.0 / M_PI) << std::endl;

  for (size_t step = 0; step < m_n_corners; step++) {
    float angle_begin = step * angle;
    float angle_end = (step + 1) * angle;

    // three corners of each slice
    m_vertexes.insert(m_vertexes.end(), {std::cos(angle_begin), 0.0f, std::sin(angle_begin)});
    m_vertexes.insert(m_vertexes.end(), {0.0f, 0.0f, 0.0f});
    m_vertexes.insert(m_vertexes.end(), {std::cos(angle_end), 0.0f, std::sin(angle_end)});
  }

  return m_vertexes;
}

int Circle::get_n_vertexes() {
  m_n_vertexes = 3 * m_n_corners;
  return m_n_vertexes;
}
