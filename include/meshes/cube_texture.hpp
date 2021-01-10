#ifndef CUBE_TEXTURE_HPP
#define CUBE_TEXTURE_HPP

#include <meshes/cube.hpp>

class CubeTexture : public Cube {
public:
  CubeTexture(const Program& program, const std::vector<std::string>& paths_images);
  void free() override;

protected:
  void set_attribute(const Program& program) override;

private:
  GLuint m_texture;

  void apply_texture(const std::vector<std::string>& paths_images);
};

#endif // CUBE_TEXTURE_HPP
