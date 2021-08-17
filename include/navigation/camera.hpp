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

  Camera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up);
  glm::mat4 get_view();
  glm::vec3 get_position() const;
  float get_fov() const;
  void move(Direction direction);
  void rotate(float x_offset, float y_offset);
  void zoom(Zoom direction);

private:
  // look at parameters
  glm::vec3 m_position;
  glm::vec3 m_direction;
  glm::vec3 m_up;

  // zoom-in corresponds to lower field-of-view
  float m_fov;

  // direction of movement
  glm::vec3 m_forward_dir;
};

#endif // CAMERA_HPP
