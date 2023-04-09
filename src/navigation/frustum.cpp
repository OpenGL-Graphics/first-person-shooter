#include <iostream>
#include <glm/gtx/string_cast.hpp>

#include "navigation/frustum.hpp"
#include "entries/target_entry.hpp"

using namespace math;

/* Same parameters as projection matrix calculated with glm::perspective() */
Frustum::Frustum(float n, float f, float aspect):
  near(n),
  far(f),
  aspect_ratio(aspect)
{
}

void Frustum::print_planes() {
  std::cout << "near.n: " << glm::to_string(near_plane.normal) << " - d: " << near_plane.d << '\n';
  std::cout << "far.n: " << glm::to_string(far_plane.normal) << " - d: " << far_plane.d << '\n';

  std::cout << "top.n: " << glm::to_string(top_plane.normal) << " - d: " << top_plane.d << '\n';
  std::cout << "bottom.n: " << glm::to_string(bottom_plane.normal) << " - d: " << bottom_plane.d << '\n';

  std::cout << "left.n: " << glm::to_string(left_plane.normal) << " - d: " << left_plane.d << '\n';
  std::cout << "right.n: " << glm::to_string(right_plane.normal) << " - d: " << right_plane.d << '\n';
}

/* All frustum planes normals point to frustum interior (to determine points inside/outside frustum) */
void Frustum::calculate_planes(const Camera& camera) {
  // near/far planes
  glm::vec3 vec_far_center = far * camera.direction;
  glm::vec3 center_near = camera.position + near * camera.direction,
            center_far = camera.position + vec_far_center;
  near_plane = Plane(camera.direction, center_near);
  far_plane = Plane(-camera.direction, center_far);

  // half size of camera's far rectangle from side view
  float fov = glm::radians(camera.fov);
  float half_height_far = far * std::tan(fov / 2);
  float half_width_far = aspect_ratio * half_height_far; // from front view
  glm::vec3 camera_right = camera.get_right();

  // vectors around frustum from perspective view
  glm::vec3 vec_far_top = vec_far_center + half_height_far * camera.up;
  glm::vec3 vec_far_bottom = vec_far_center - half_height_far * camera.up;
  glm::vec3 vec_far_right = vec_far_center + half_width_far * camera_right;
  glm::vec3 vec_far_left = vec_far_center - half_width_far * camera_right;

  // top/bottom planes
  glm::vec3 normal_top = glm::normalize(glm::cross(vec_far_top, camera_right));
  glm::vec3 normal_bottom = glm::normalize(glm::cross(camera_right, vec_far_bottom));
  top_plane = Plane(normal_top, camera.position);
  bottom_plane = Plane(normal_bottom, camera.position);

  // right/left planes
  glm::vec3 normal_right = glm::normalize(glm::cross(camera.up, vec_far_right));
  glm::vec3 normal_left = glm::normalize(glm::cross(vec_far_left, camera.up));
  right_plane = Plane(normal_right, camera.position);
  left_plane = Plane(normal_left, camera.position);
}

/**
 * If element is a:
 * point: True if given point is inside frustum (i.e. in front of all frustum planes)
 * bbox: True if bbox's half diagonal (rel. to each plane) is inside frustum
 */
template <typename T>
bool Frustum::is_inside(const T& element) const {
  return (
    left_plane.is_in_front_of_plane(element) &&
    right_plane.is_in_front_of_plane(element) &&
    top_plane.is_in_front_of_plane(element) &&
    bottom_plane.is_in_front_of_plane(element) &&
    near_plane.is_in_front_of_plane(element) &&
    far_plane.is_in_front_of_plane(element)
  );
}

/**
 * Fiter out matrices corresp. to positions outside frustum
 * Used to frustum cull tiles from level renderers (tile = wall, window, door, tree...)
 * Note: matrices and positions have the same size. Positions can be extracted from model but not from normal matrix!
 * @param elements To check if they are inside frustum (can be points or bboxes)
 */
template <typename T, typename U>
std::vector<T> Frustum::cull(const std::vector<T>& vec, const std::vector<U>& elements) const {
  std::vector<T> vec_out;

  for (size_t i_element = 0; i_element < elements.size(); ++i_element) {
    T item = vec[i_element];
    U element = elements[i_element];

    // check if inside frustum
    if (is_inside(element))
      vec_out.push_back(item);
  }

  return vec_out;
}

// template instantiation (avoids linking error)
template std::vector<glm::mat4> Frustum::cull(const std::vector<glm::mat4>&, const std::vector<glm::vec3>&) const;
template std::vector<TargetEntry> Frustum::cull(const std::vector<TargetEntry>&, const std::vector<glm::vec3>&) const;

template std::vector<glm::mat4> Frustum::cull(const std::vector<glm::mat4>&, const std::vector<BoundingBox>&) const;
template std::vector<TargetEntry> Frustum::cull(const std::vector<TargetEntry>&, const std::vector<BoundingBox>&) const;
