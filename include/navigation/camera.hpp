// Inspired by https://learnopengl.com/Getting-started/Camera
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <navigation/direction.hpp>
#include <navigation/zoom.hpp>

class Camera {
public:
  Camera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up);
  glm::mat4 get_view() const;
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

  // vertical/horizontal angle (rotation around x-axis/y-axis)
  float m_pitch;
  float m_yaw;

  // movement constants
  const float X_SPEED = 0.1f;
  const float Z_SPEED = 0.1f;
  const float SENSITIVITY = 0.01f;

  // zoom-in corresponds to lower field-of-view
  float m_fov;
};

#endif // CAMERA_HPP
