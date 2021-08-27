// Inspired by https://learnopengl.com/Getting-started/Camera
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

#include "navigation/direction.hpp"
#include "navigation/zoom.hpp"

struct Camera {
  /**
   * Vertical/horizontal angles in radian (rotation around x-axis/y-axis)
   * y-angle used to tie PC's directions of movement to camera (not world axes)
   */
  float pitch;
  float yaw;

  /* used in 3D projection matrix (zoom-in corresponds to lower field-of-view) */
  float fov;

  /* look at parameters (used to position cube on LMB click) */
  glm::vec3 position;
  glm::vec3 direction;

  Camera(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& up);
  glm::mat4 get_view();
  void move(Direction d);
  void rotate(float x_offset, float y_offset);
  void zoom(Zoom z);

private:
  glm::vec3 m_up;

  // direction of movement
  glm::vec3 m_forward_dir;
};

#endif // CAMERA_HPP
