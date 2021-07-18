#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <geometries/geometry.hpp>

class Sphere : public Geometry {
public:
  Sphere(int n_longitudes, int n_latitudes);
  virtual std::vector<float> get_vertexes() const override;
  virtual unsigned int get_n_elements() const override;

private:
  int m_n_longitudes;
  int m_n_latitudes;
  void set_vertexes();
  void set_n_elements();
};

#endif // SPHERE_HPP
