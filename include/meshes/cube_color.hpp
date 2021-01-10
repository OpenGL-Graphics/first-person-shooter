#ifndef CUBE_COLOR_HPP
#define CUBE_COLOR_HPP

#include <meshes/cube.hpp>

class CubeColor : public Cube {
public:
  CubeColor(const Program& program);

protected:
  void set_attribute(const Program& program) override;
};

#endif // CUBE_COLOR_HPP
