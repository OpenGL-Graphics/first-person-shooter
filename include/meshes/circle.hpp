#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include <meshes/mesh.hpp>
#include <cmath>

class Circle : public Mesh {
public:
  Circle(const Program& program, int n_corners);

protected:
  std::vector<float> m_vertexes;
  int m_n_vertexes;

  void set_attribute(const Program& program) override;
  std::vector<float> get_vertexes() override;
  int get_n_vertexes() override;

private:
  int m_n_corners;
};

#endif // CIRCLE_HPP
