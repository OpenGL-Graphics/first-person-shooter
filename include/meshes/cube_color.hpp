#ifndef CUBE_COLOR_HPP
#define CUBE_COLOR_HPP

#include <meshes/cube.hpp>

class CubeColor : public Cube {
public:
  CubeColor(Program program);

protected:
  void set_attribute() override;
};

#endif // CUBE_COLOR_HPP
