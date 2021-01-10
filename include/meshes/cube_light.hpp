#ifndef CUBE_LIGHT_HPP
#define CUBE_LIGHT_HPP

#include <meshes/cube.hpp>

class CubeLight : public Cube {
public:
  CubeLight(const Program& program);

protected:
  void set_attribute(const Program& program) override;
};

#endif // CUBE_LIGHT_HPP
