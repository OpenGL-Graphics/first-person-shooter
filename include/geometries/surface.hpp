#ifndef SURFACE_HPP
#define SURFACE_HPP

#include <geometries/geometry.hpp>

class Surface : public Geometry {
public:
  Surface();
  virtual std::vector<float> get_vertexes() const;
  virtual int get_n_vertexes() const;

protected:
  std::vector<float> m_vertexes;
  int m_n_vertexes;

private:
  static const std::vector<float> VERTEXES;
};

#endif // SURFACE_HPP
