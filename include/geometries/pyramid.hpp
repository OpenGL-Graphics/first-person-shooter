#ifndef PYRAMID_HPP
#define PYRAMID_HPP

#include <geometries/geometry.hpp>

namespace geometry {
  class Pyramid : public Geometry {
  public:
    virtual std::vector<float> get_vertexes() const;
    virtual int get_n_vertexes() const;

  protected:
    // coord(x,y,z)        normal(nx,ny,nz)
    std::vector<float> m_vertexes {
      // front-right face
       0.0f, 0.0f,  1.0f,  1.0f,  0.0f, 1.0f,
       1.0f, 0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
       0.0f, 1.0f,  0.0f,  1.0f,  0.0f, 1.0f,

      // front-left face
       0.0f, 0.0f,  1.0f, -1.0f,  0.0f, 1.0f,
      -1.0f, 0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
       0.0f, 1.0f,  0.0f, -1.0f,  0.0f, 1.0f,

      // back-right face
       1.0f, 0.0f,  0.0f,  1.0f,  0.0f, -1.0f,
       0.0f, 0.0f, -1.0f,  1.0f,  0.0f, -1.0f,
       0.0f, 1.0f,  0.0f,  1.0f,  0.0f, -1.0f,

      // back-left face
       0.0f, 0.0f, -1.0f, -1.0f,  0.0f, -1.0f,
      -1.0f, 0.0f,  0.0f, -1.0f,  0.0f, -1.0f,
       0.0f, 1.0f,  0.0f, -1.0f,  0.0f, -1.0f,

      // bottom face
       1.0f, 0.0f,  0.0f,  0.0f, -1.0f, 0.0f,
       0.0f, 0.0f,  1.0f,  0.0f, -1.0f, 0.0f,
      -1.0f, 0.0f,  0.0f,  0.0f, -1.0f, 0.0f,
       1.0f, 0.0f,  0.0f,  0.0f, -1.0f, 0.0f,
       0.0f, 0.0f, -1.0f,  0.0f, -1.0f, 0.0f,
      -1.0f, 0.0f,  0.0f,  0.0f, -1.0f, 0.0f,
    };
    int m_n_vertexes = 6 * 3;
  };
}

#endif // PYRAMID_HPP
