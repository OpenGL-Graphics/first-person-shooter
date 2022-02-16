#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <geometries/geometry.hpp>

class Sphere : public Geometry {
public:
  Sphere(int n_longitudes, int n_latitudes);

  ///
  std::vector<float> get_vertexes() const override;
  unsigned int get_n_elements() const override;
  // std::vector<unsigned int> get_indices() const override;
  // std::vector<glm::vec3> get_positions() const override;
  std::vector<glm::vec3> get_positions();
  ///

private:
  int m_n_longitudes;
  int m_n_latitudes;

  void set_vertexes();
  // void set_n_elements();
};

#endif // SPHERE_HPP
