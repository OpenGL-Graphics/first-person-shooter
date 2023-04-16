/* https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling */
#ifndef FRUSTUM_HPP
#define FRUSTUM_HPP

#include <vector>

#include "math/bounding_box.hpp"
#include "math/plane.hpp"
#include "navigation/camera.hpp"

class Frustum {
public:
  Frustum(float n, float f, float aspect);
  void calculate_planes(const Camera& camera);
  void print_planes();

  template <typename T>
  bool is_inside(const T& element) const;

  template <typename T>
  std::vector<T> cull(const std::vector<T>& arr, const std::vector<BoundingBox>& bboxes) const;

private:
  float near;
  float far;
  float aspect_ratio;

  math::Plane left_plane;
  math::Plane right_plane;
  math::Plane top_plane;
  math::Plane bottom_plane;
  math::Plane near_plane;
  math::Plane far_plane;
};

#endif // FRUSTUM_HPP
