#ifndef CYLINDER_HPP
#define CYLINDER_HPP

#include <geometries/geometry.hpp>

class Cylinder : public Geometry {
public:
  Cylinder(int n_corners);
  virtual std::vector<float> get_vertexes() const;
  virtual int get_n_vertexes() const;

private:
  int m_n_corners;
  void set_vertexes();
  void set_n_vertexes();
};

#endif // CYLINDER_HPP
