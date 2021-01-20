#include <geometries/sphere.hpp>
#include <iostream>
#include <cmath>

Sphere::Sphere(int n_longitudes, int n_latitudes):
  m_n_longitudes(n_longitudes),
  m_n_latitudes(n_latitudes)
{
  // calculate vertexes coords on creation
  set_vertexes();
  set_n_vertexes();
}

void Sphere::set_vertexes() {
  // lon in [0, 2pi] and lat in [0, pi]
  float angle_lon = 2*M_PI / m_n_longitudes;
  float angle_lat = M_PI / m_n_latitudes;

  // parameteric equation for a sphere depends on lon & lat angles
  for (size_t step_lat = 0; step_lat < m_n_latitudes; step_lat++) {
    float lat_begin = step_lat * angle_lat;
    float lat_end = (step_lat + 1) * angle_lat;

    for (size_t step_lon = 0; step_lon < m_n_longitudes; step_lon++) {
      float lon_begin = step_lon * angle_lon;
      float lon_end = (step_lon + 1) * angle_lon;
      m_vertexes.insert(m_vertexes.end(), {std::cos(lon_begin)*std::sin(lat_begin), std::cos(lat_begin), std::sin(lon_begin)*std::sin(lat_begin)});
      m_vertexes.insert(m_vertexes.end(), {std::cos(lon_end)*std::sin(lat_begin), std::cos(lat_begin), std::sin(lon_end)*std::sin(lat_begin)});
      m_vertexes.insert(m_vertexes.end(), {std::cos(lon_end)*std::sin(lat_end), std::cos(lat_end), std::sin(lon_end)*std::sin(lat_end)});

      m_vertexes.insert(m_vertexes.end(), {std::cos(lon_begin)*std::sin(lat_begin), std::cos(lat_begin), std::sin(lon_begin)*std::sin(lat_begin)});
      m_vertexes.insert(m_vertexes.end(), {std::cos(lon_end)*std::sin(lat_end), std::cos(lat_end), std::sin(lon_end)*std::sin(lat_end)});
      m_vertexes.insert(m_vertexes.end(), {std::cos(lon_begin)*std::sin(lat_end), std::cos(lat_end), std::sin(lon_begin)*std::sin(lat_end)});
    }
  }
}

void Sphere::set_n_vertexes() {
  m_n_vertexes = m_n_longitudes * m_n_latitudes * 2 * 3;
}

std::vector<float> Sphere::get_vertexes() const {
  return m_vertexes;
}

int Sphere::get_n_vertexes() const {
  return m_n_vertexes;
}
