#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include <meshes/mesh.hpp>
#include <materials/texture.hpp>

class Rectangle : public Mesh {
public:
  Rectangle(const Program& program, const Texture& texture);

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

private:
  Texture m_texture;
  std::string m_path_image;

  void apply_texture();
};

#endif // RECTANGLE_HPP
