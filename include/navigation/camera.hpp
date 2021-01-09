// Inspired by https://learnopengl.com/Getting-started/Camera
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class Direction {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
};

class Camera {
public:
  Camera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up):
    m_position(position),
    m_direction(direction),
    m_up(up),
    m_pitch(0.0f),
    m_yaw(0.0f)
  {
  }

  glm::mat4 get_view_mat() {
    // consider rotation due to mouse (yaw & pitch)
    glm::mat4 view_mat = glm::lookAt(m_position, m_position + m_direction, m_up);
    view_mat = glm::rotate(view_mat, m_pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    view_mat = glm::rotate(view_mat, m_yaw, glm::vec3(0.0f, 1.0f, 0.0f));

    return view_mat;
  }

  void move(Direction direction) {
    if (direction == Direction::FORWARD)
      m_position -= Z_SPEED * glm::vec3(0.0f, 0.0f, 1.0f);
    if (direction == Direction::BACKWARD)
      m_position += Z_SPEED * glm::vec3(0.0f, 0.0f, 1.0f);
    if (direction == Direction::LEFT)
      m_position -= X_SPEED * glm::vec3(1.0f, 0.0f, 0.0f);
    if (direction == Direction::RIGHT)
      m_position += X_SPEED * glm::vec3(1.0f, 0.0f, 0.0f);
  }
 
  void rotate(float x_offset, float y_offset) {
    m_yaw += SENSITIVITY * x_offset;
    m_pitch += SENSITIVITY * y_offset;

    // limit horizontal/vertical rotation angle
    m_yaw = (m_yaw > glm::radians(90.0f)) ? glm::radians(90.0f): m_yaw;
    m_yaw = (m_yaw < glm::radians(-90.0f)) ? glm::radians(-90.0f): m_yaw;
    m_pitch = (m_pitch > glm::radians(90.0f)) ? glm::radians(90.0f): m_pitch;
    m_pitch = (m_pitch < glm::radians(-90.0f)) ? glm::radians(-90.0f): m_pitch;
  }

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
};

#endif // CAMERA_HPP
