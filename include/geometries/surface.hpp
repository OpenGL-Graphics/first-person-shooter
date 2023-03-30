#ifndef SURFACE_HPP
#define SURFACE_HPP

#include "geometries/geometry.hpp"

namespace geometry {
  class Surface : public Geometry {
  public:
    Surface(const glm::vec2& size={1.0f, 1.0f});
    Surface(const std::vector<float>& vertexes);

  private:
    static const std::vector<unsigned int> INDICES;
    static const std::vector<glm::vec3> POSITIONS;

    /**
     * Prevent texture from stretching by pushing uv-coord outside [0, 1] (i.e. repeat)
     * No need to scale later (already baked in vertex xyz coords)
     */
    glm::vec2 m_size;
    std::vector<float> _get_vertexes();
    void set_n_elements();
  };
}

#endif // SURFACE_HPP
