#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <geometries/geometry.hpp>

class Sphere : public Geometry {
public:
  Sphere(int n_longitudes, int n_latitudes);
  virtual std::vector<float> get_vertexes() const;
  virtual int get_n_vertexes() const;

private:
  int m_n_longitudes;
  int m_n_latitudes;
  void set_vertexes();
  void set_n_vertexes();
};

#endif // SPHERE_HPP
