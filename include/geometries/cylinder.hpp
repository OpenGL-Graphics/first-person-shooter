#ifndef CYLINDER_HPP
#define CYLINDER_HPP

#include <geometries/geometry.hpp>

class Cylinder : public Geometry {
public:
  Cylinder(int n_corners);
  virtual std::vector<float> get_vertexes() const override;
  virtual unsigned int get_n_elements() const override;

private:
  int m_n_corners;
  void set_vertexes();
  void set_n_elements();
};

#endif // CYLINDER_HPP
