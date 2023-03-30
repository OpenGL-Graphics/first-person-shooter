#include "geometries/gizmo.hpp"

using namespace geometry;

Gizmo::Gizmo() {
  set_vertexes();
  set_indices();
  set_n_elements();
}

/* 4 points needed to make 3 axes xyz (origin and 3 others) */
void Gizmo::set_vertexes() {
  m_vertexes = {
    0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
  };
}

void Gizmo::set_indices() {
  m_indices = {
    0, 1, // x-axis
    0, 2, // y-axis
    0, 3, // z-axis
  };
}

/**
 * @return # of indices to render (incl. repeated indices)
 * 3 line segments (i.e. axes), each having two points extremities
 */
void Gizmo::set_n_elements() {
  m_n_elements = 6;
}
