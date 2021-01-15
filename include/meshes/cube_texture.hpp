#ifndef CUBE_TEXTURE_HPP
#define CUBE_TEXTURE_HPP

#include <meshes/cube.hpp>
#include <materials/texture.hpp>

class CubeTexture : public Cube {
public:
  CubeTexture(const Program& program, const Texture& texture);

protected:
  void set_attribute() override;

private:
  Texture m_texture;
};

#endif // CUBE_TEXTURE_HPP
