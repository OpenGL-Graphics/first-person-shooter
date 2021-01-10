#ifndef CUBE_LIGHT_HPP
#define CUBE_LIGHT_HPP

#include <meshes/cube.hpp>

class CubeLight : public Cube {
public:
  CubeLight(Program program);

protected:
  void set_attribute() override;
};

#endif // CUBE_LIGHT_HPP
