#include <iostream>
#include <cmath>

#include "geometries/sphere.hpp"

Sphere::Sphere(int n_longitudes, int n_latitudes):
  m_n_longitudes(n_longitudes),
  m_n_latitudes(n_latitudes)
{
  // calculate vertexes & indices on creation
  set_vertexes();
  // set_n_elements();
}

/**
 * Sets vertexes & indices for unit sphere (radius = 1)
 * Parametric equation for a sphere depends on lon (theta) & lat (phi) angles
 * See spherical coords: https://mathworld.wolfram.com/SphericalCoordinates.html
 * TODO: vertexes belonging to adjacent faces still duplicated!
 */
void Sphere::set_vertexes() {
  // angles lon in [0, 2pi] rad and lat in [0, pi] rad
  float lon_step = 2*M_PI / m_n_longitudes;
  float lat_step = M_PI / m_n_latitudes;

  // angles rel. to vertical y-axis (phi angle) - start at north-pole & go downwards
  for (size_t i_lat = 0; i_lat < m_n_latitudes; i_lat++) {
    float lat_begin = i_lat * lat_step;
    float lat_end = (i_lat + 1) * lat_step;

    // angles rel. to x-axis (theta angle) - start at x-axis & rotate ccw
    for (size_t i_lon = 0; i_lon < m_n_longitudes; i_lon++) {
      float lon_begin = i_lon * lon_step;
      float lon_end = (i_lon + 1) * lon_step;

      // next vertex indice to insert from last one inserted (start from 0)
      unsigned int indice_next = (!m_indices.empty()) ? m_indices.back() + 1 : 0;

      // face points
      /*
         p0 p1
         x--x
         |\ |
         | \|
         x--x
         p3 p2
      */
      float p0[] = { std::cos(lon_begin)*std::sin(lat_begin), std::cos(lat_begin), std::sin(lon_begin)*std::sin(lat_begin) };
      float p1[] = { std::cos(lon_end)*std::sin(lat_begin), std::cos(lat_begin), std::sin(lon_end)*std::sin(lat_begin) };
      float p2[] = { std::cos(lon_end)*std::sin(lat_end), std::cos(lat_end), std::sin(lon_end)*std::sin(lat_end) };
      float p3[] = { std::cos(lon_begin)*std::sin(lat_end), std::cos(lat_end), std::sin(lon_begin)*std::sin(lat_end) };

      // triangle instead of face at north-pole (otherwise p0 & p1 are duplicated)
      if (i_lat == 0) {
        m_vertexes.insert(m_vertexes.end(), p0, p0 + 3);
        m_vertexes.insert(m_vertexes.end(), p2, p2 + 3);
        m_vertexes.insert(m_vertexes.end(), p3, p3 + 3);
        m_indices.insert(m_indices.end(), { indice_next, indice_next + 1, indice_next + 2 }); // initially: 0, 2, 3
        continue;
      }

      // triangle instead of face at south-pole (otherwise p3 & p2 are duplicated)
      if (i_lat == m_n_latitudes - 1) {
        m_vertexes.insert(m_vertexes.end(), p0, p0 + 3);
        m_vertexes.insert(m_vertexes.end(), p1, p1 + 3);
        m_vertexes.insert(m_vertexes.end(), p2, p2 + 3);
        m_indices.insert(m_indices.end(), { indice_next, indice_next + 1, indice_next + 2 }); // initially: 0, 2, 3
        continue;
      }

      // 1st triangle in face (general case)
      m_vertexes.insert(m_vertexes.end(), p0, p0 + 3);
      m_vertexes.insert(m_vertexes.end(), p1, p1 + 3);
      m_vertexes.insert(m_vertexes.end(), p2, p2 + 3);
      m_indices.insert(m_indices.end(), { indice_next, indice_next + 1, indice_next + 2 }); // initially: 0, 1, 2

      // 2nd triangle in face (general case)
      m_vertexes.insert(m_vertexes.end(), p3, p3 + 3);
      m_indices.insert(m_indices.end(), { indice_next, indice_next + 2, indice_next + 3 }); // initially: 0, 2, 3
    }
  }

  std::cout << "# of vertexes coords: " << m_vertexes.size() << '\n';
}

/*
void Sphere::set_n_elements() {
  m_n_elements = m_n_longitudes * m_n_latitudes * 2 * 3;
}
*/

std::vector<float> Sphere::get_vertexes() const {
  return m_vertexes;
}

unsigned int Sphere::get_n_elements() const {
  // return m_n_elements;
  return m_indices.size();
}
