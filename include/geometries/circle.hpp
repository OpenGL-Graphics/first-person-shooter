#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include <geometries/geometry.hpp>

namespace geometry {
  class Circle : public Geometry {
  public:
    Circle(int n_corners);
    virtual std::vector<float> get_vertexes() const override;
    virtual unsigned int get_n_elements() const override;

  private:
    int m_n_corners;
    void set_vertexes();
    void set_n_elements();
  };
}

#endif // CIRCLE_HPP
