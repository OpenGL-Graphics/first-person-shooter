#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <meshes/mesh.hpp>
#include <cmath>

class Sphere : public Mesh {
public:
  Sphere(const Program& program, int n_longitudes, int n_latitudes);

protected:
  std::vector<float> m_vertexes;
  int m_n_vertexes;

  void set_attribute(const Program& program) override;
  std::vector<float> get_vertexes() override;
  int get_n_vertexes() override;

private:
  int m_n_longitudes;
  int m_n_latitudes;
};

#endif // SPHERE_HPP
