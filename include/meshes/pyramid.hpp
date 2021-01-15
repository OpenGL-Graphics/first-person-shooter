#ifndef PYRAMID_HPP
#define PYRAMID_HPP

#include <meshes/mesh.hpp>

class Pyramid : public Mesh {
public:
  Pyramid(const Program& program);

protected:
  void set_attribute() override;

private:
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

  std::vector<float> get_vertexes() override;
  int get_n_vertexes() override;
};

#endif // PYRAMID_HPP
