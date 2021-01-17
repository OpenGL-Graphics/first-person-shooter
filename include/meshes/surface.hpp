#ifndef SURFACE_HPP
#define SURFACE_HPP

#include <meshes/mesh.hpp>

class Surface : public Mesh {
public:
  Surface(const Program& program);

protected:
  // coord(x,y,z)         texture(u,v,w)
  std::vector<float> m_vertexes {
    // positive-z (front face)
    -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
  };
  int m_n_vertexes = 6;

  void set_attribute() override;
  std::vector<float> get_vertexes() override;
  int get_n_vertexes() override;
};

#endif // SURFACE_HPP
