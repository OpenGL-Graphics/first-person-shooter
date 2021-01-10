#ifndef PYRAMID_HPP
#define PYRAMID_HPP

#include <meshes/mesh.hpp>

class Pyramid : public Mesh {
public:
  Pyramid(const Program& program);

protected:
  void set_attribute(const Program& program) override;

private:
  static const std::vector<float> VERTEXES;
  static const int N_VERTEXES = 6 * 3;
};

#endif // PYRAMID_HPP
