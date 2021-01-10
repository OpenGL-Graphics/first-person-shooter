#ifndef CUBE_HPP
# define CUBE_HPP

#include <meshes/mesh.hpp>

class Cube : public Mesh {
public:
  Cube(Program program);

protected:
  virtual void set_attribute();

private:
  static const std::vector<float> VERTEXES;
};

#endif // CUBE_HPP
