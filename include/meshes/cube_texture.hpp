#ifndef CUBE_TEXTURE_HPP
#define CUBE_TEXTURE_HPP

#include <meshes/cube.hpp>

class CubeTexture : public Cube {
public:
  CubeTexture(const Program& program);

protected:
  void set_attribute() override;
};

#endif // CUBE_TEXTURE_HPP
