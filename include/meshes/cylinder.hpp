#ifndef CYLINDER_HPP
#define CYLINDER_HPP

#include <meshes/mesh.hpp>
#include <cmath>

class Cylinder : public Mesh {
public:
  Cylinder(const Program& program, int n_corners);

protected:
  std::vector<float> m_vertexes;
  int m_n_vertexes;

  void set_attribute() override;
  std::vector<float> get_vertexes() override;
  int get_n_vertexes() override;

private:
  int m_n_corners;
};

#endif // CYLINDER_HPP
