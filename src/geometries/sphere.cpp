#include <iostream>
#include <cmath>

#include "geometries/sphere.hpp"

Sphere::Sphere(unsigned int n_longitudes, unsigned int n_latitudes):
  m_n_longitudes(n_longitudes),
  m_n_latitudes(n_latitudes)
{
  // calculate vertexes & indices on creation
  set_vertexes();
  set_indices();
}

/**
 * Sets vertexes for unit sphere (radius = 1)
 * Parametric equation for a sphere depends on lon (theta) & lat (phi) angles
 * See spherical coords: https://mathworld.wolfram.com/SphericalCoordinates.html
 */
void Sphere::set_vertexes() {
  // north-pole
  m_vertexes.insert(m_vertexes.end(), { 0.0f, 1.0f, 0.0f });

  // vertexes belonging to horizontal circles between north & south poles
  // angles lon in [0, 2pi] rad and lat in [0, pi] rad
  float lon_step = 2*M_PI / m_n_longitudes;
  float lat_step = M_PI / m_n_latitudes;

  // angles rel. to vertical y-axis (phi angle) - start at north-pole & go downwards
  for (size_t i_lat = 1; i_lat < m_n_latitudes; i_lat++) {
    float lat = i_lat * lat_step;

    // angles rel. to x-axis (theta angle) - start at x-axis & rotate ccw
    for (size_t i_lon = 0; i_lon < m_n_longitudes; i_lon++) {
      float lon = i_lon * lon_step;

      float point[] = { std::cos(lon)*std::sin(lat), std::cos(lat), std::sin(lon)*std::sin(lat) };
      m_vertexes.insert(m_vertexes.end(), point, point + 3);
    }
  }

  // south-pole
  m_vertexes.insert(m_vertexes.end(), { 0.0f, -1.0f, 0.0f });

  std::cout << "# of vertexes coords: " << m_vertexes.size() << '\n';
}

/* Proceed from north-pole downwards towards south-pole */
void Sphere::set_indices() {
  // triangles from north-pole to horizontal circle just below it
  for (unsigned int i_lon = 1; i_lon < m_n_longitudes; i_lon++) {
    m_indices.insert(m_indices.end(), { 0, i_lon, i_lon + 1 });
  }

  // close cycle at very top with last triangle
  m_indices.insert(m_indices.end(), { 0, 1, m_n_longitudes });


  // start from first horizontal circle below south-pole & draw triangle between it & following one
  /*
     p1 p0
     x--x
     |\ |
     | \|
     x--x
     p3 p2
  */
  for (unsigned int i_lat = 0; i_lat < m_n_latitudes - 2; i_lat++) {
    for (unsigned int i_lon = 1; i_lon < m_n_longitudes; i_lon++) {
      unsigned int i_p0 = i_lon                    + i_lat*m_n_longitudes;
      unsigned int i_p1 = i_p0 + 1;
      unsigned int i_p2 = (i_lon + m_n_longitudes) + i_lat*m_n_longitudes;
      unsigned int i_p3 = i_p2 + 1;

      // two triangles in same face
      m_indices.insert(m_indices.end(), { i_p0, i_p1, i_p2 });
      m_indices.insert(m_indices.end(), { i_p1, i_p2, i_p3 });
    }

    // close cycle with last face
    unsigned int i_p0 = 1              + i_lat*m_n_longitudes;
    unsigned int i_p1 = m_n_longitudes + i_lat*m_n_longitudes;
    unsigned int i_p2 = i_p0 + m_n_longitudes;
    unsigned int i_p3 = i_p2 + m_n_longitudes;

    m_indices.insert(m_indices.end(), { i_p0, i_p1, i_p2 });
    m_indices.insert(m_indices.end(), { i_p1, i_p2, i_p3 });
  }


  // triangles from second-to-last horizontal circle to south-pole
  unsigned int indice_last = m_n_longitudes * (m_n_latitudes - 1) + 1;
  for (unsigned int i_lon = 1; i_lon < m_n_longitudes; i_lon++) {
    m_indices.insert(m_indices.end(), { indice_last, indice_last - i_lon, indice_last - (i_lon + 1) });
  }

  // close cycle at very bottom with last triangle
  m_indices.insert(m_indices.end(), { indice_last, indice_last - 1, indice_last - m_n_longitudes });
}

std::vector<float> Sphere::get_vertexes() const {
  return m_vertexes;
}

unsigned int Sphere::get_n_elements() const {
  return m_indices.size();
}
