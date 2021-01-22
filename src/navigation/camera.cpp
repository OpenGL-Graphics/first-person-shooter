#include <navigation/camera.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up):
  m_position(position),
  m_direction(direction),
  m_up(up),
  m_pitch(0.0f),
  m_yaw(0.0f),
  m_fov(45.0f)
{
}

glm::mat4 Camera::get_view() const {
  // consider rotation due to mouse (yaw & pitch)
  glm::mat4 view = glm::lookAt(m_position, m_position + m_direction, m_up);
  view = glm::rotate(view, m_pitch, glm::vec3(1.0f, 0.0f, 0.0f));
  view = glm::rotate(view, m_yaw, glm::vec3(0.0f, 1.0f, 0.0f));

  return view;
}

glm::vec3 Camera::get_position() const {
  // used to calculate specular component of material
  return m_position;
}

float Camera::get_fov() const {
  // used to zoom in/out using mouse wheel
  return m_fov;
}

void Camera::zoom(Zoom direction) {
  if (direction == Zoom::IN) {
    m_fov -= 5.0f;
  } else if (direction == Zoom::OUT) {
    m_fov += 5.0f;
  }
}

void Camera::move(Direction direction) {
  if (direction == Direction::FORWARD)
    m_position -= Z_SPEED * glm::vec3(0.0f, 0.0f, 1.0f);
  if (direction == Direction::BACKWARD)
    m_position += Z_SPEED * glm::vec3(0.0f, 0.0f, 1.0f);
  if (direction == Direction::LEFT)
    m_position -= X_SPEED * glm::vec3(1.0f, 0.0f, 0.0f);
  if (direction == Direction::RIGHT)
    m_position += X_SPEED * glm::vec3(1.0f, 0.0f, 0.0f);
}

void Camera::rotate(float x_offset, float y_offset) {
  m_yaw += SENSITIVITY * x_offset;
  m_pitch += SENSITIVITY * y_offset;

  // limit horizontal/vertical rotation angle
  m_yaw = (m_yaw > glm::radians(90.0f)) ? glm::radians(90.0f): m_yaw;
  m_yaw = (m_yaw < glm::radians(-90.0f)) ? glm::radians(-90.0f): m_yaw;
  m_pitch = (m_pitch > glm::radians(90.0f)) ? glm::radians(90.0f): m_pitch;
  m_pitch = (m_pitch < glm::radians(-90.0f)) ? glm::radians(-90.0f): m_pitch;
}
