#ifndef CUBE_HPP
#define CUBE_HPP

#include <geometries/geometry.hpp>

class Cube : public Geometry {
public:
  Cube();
  virtual std::vector<float> get_vertexes() const;
  virtual int get_n_vertexes() const;

protected:
  std::vector<float> m_vertexes;
  int m_n_vertexes;

private:
  static const std::vector<float> VERTEXES;
};

#endif // CUBE_HPP
