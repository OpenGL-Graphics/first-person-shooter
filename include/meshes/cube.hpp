#ifndef CUBE_HPP
# define CUBE_HPP

#include <meshes/mesh.hpp>

class Cube : public Mesh {
public:
  Cube(const Program& program);

protected:
  void set_attribute(const Program& program) override;

private:
  static const std::vector<float> VERTEXES;
  static const int N_VERTEXES = 6 * 2 * 3;
};

#endif // CUBE_HPP
