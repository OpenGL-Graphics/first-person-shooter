#include "geometries/grid_lines.hpp"

GridLines::GridLines() {
  set_vertexes();
  set_indices();
  set_n_elements();
}

void GridLines::set_vertexes() {
  // `n_lines` start points for lines parallel to x-axis
  for (int i_line = 0; i_line < N_LINES; ++i_line)
    m_vertexes.insert(m_vertexes.end(), { 0, 0, static_cast<float>(i_line) });

  // `n_lines` end points for lines parallel to x-axis
  for (int i_line = 0; i_line < N_LINES; ++i_line)
    m_vertexes.insert(m_vertexes.end(), { N_LINES - 1, 0, static_cast<float>(i_line) });

  // `n_lines - 2` start points for lines parallel to z-axis
  // first and last points already included in 1st & 2nd loop
  for (int i_line = 1; i_line < N_LINES - 1; ++i_line)
    m_vertexes.insert(m_vertexes.end(), { static_cast<float>(i_line), 0, 0 });

  // `n_lines - 2` start points for lines parallel to z-axis
  // first and last points already included in 1st & 2nd loop
  for (int i_line = 1; i_line < N_LINES - 1; ++i_line)
    m_vertexes.insert(m_vertexes.end(), { static_cast<float>(i_line), 0, N_LINES - 1 });
}

void GridLines::set_indices() {
  // lines (pair of points) linking points in 1st loop to 2nd one (// to x-axis)
  for (unsigned int i_line = 0; i_line < N_LINES; ++i_line)
    m_indices.insert(m_indices.end(), { i_line, i_line + N_LINES });

  // first line parallel to z-axis
  m_indices.insert(m_indices.end(), { 0, N_LINES - 1 });

  // lines (pair of points) linking points in 3rd loop to 4th one (// to z-axis)
  // first & last line drawn outside loop (bcoz of non-duplicated points)
  for (unsigned int i_line = 0; i_line < N_LINES - 2; ++i_line)
    m_indices.insert(m_indices.end(), { 2*N_LINES + i_line, 2*N_LINES + (N_LINES - 2) + i_line });

  // last line parallel to z-axis
  m_indices.insert(m_indices.end(), { N_LINES, 2*N_LINES - 1 });
}

/**
 * @return # of indices to render (incl. repeated indices)
 * `n_lines` horizontal lines and `n_lines` vertical lines, each having two points extremities
 */
void GridLines::set_n_elements() {
  int n_lines = 2 * N_LINES;
  m_n_elements = 2 * n_lines;
}
