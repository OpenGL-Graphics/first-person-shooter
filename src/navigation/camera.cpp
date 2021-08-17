#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <algorithm>

#include "navigation/camera.hpp"

// not declared as private members as constants cause class's implicit copy-constructor to be deleted (prevents re-assignment)
// movement constants
const float SPEED = 0.1f;
const float SENSITIVITY = 0.25e-2;

Camera::Camera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up):
  m_position(position),
  m_direction(direction),
  m_up(up),
  m_fov(45.0f),
  m_forward_dir(m_direction),

  pitch(0.0f),
  yaw(0.0f)
{
}

/**
 * View matrix orients a scene in such a way to simulate camera's movement (inversion of camera's transformation matrix)
 * i.e. translation/rotation of scene in opposite directions to camera
 * Important: pitch/yaw angles rotate scene's object not camera
 */
glm::mat4 Camera::get_view() {
  // only camera direction is rotated by mouse in `rotate()`
  glm::mat4 view = glm::lookAt(m_position, m_position + m_direction, m_up);

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
  glm::vec3 right_dir = glm::normalize(glm::cross(m_forward_dir, m_up));

  if (direction == Direction::FORWARD)
    m_position += SPEED * m_forward_dir;
  if (direction == Direction::BACKWARD)
    m_position -= SPEED * m_forward_dir;
  if (direction == Direction::RIGHT)
    m_position += SPEED * right_dir;
  if (direction == Direction::LEFT)
    m_position -= SPEED * right_dir;
}

void Camera::rotate(float x_offset, float y_offset) {
  // increment angles accord. to 2D mouse movement
  float PI = glm::pi<float>();
  float yaw_offset = SENSITIVITY * x_offset;
  float pitch_offset = SENSITIVITY * y_offset;
  yaw += yaw_offset;
  pitch += pitch_offset;

  // yaw in [-2pi, 2pi] & clamp pitch angle to prevent weird rotation of camera when pitch ~ pi/2
  yaw = std::fmod(yaw, 2 * PI);
  pitch = std::clamp(pitch, glm::radians(-60.0f), glm::radians(60.0f));
  if (pitch == glm::radians(-60.0f) || pitch == glm::radians(60.0f)) {
    return;
  }

  // fps camera navigation by rotating its direction vector around x/y-axis
  glm::vec3 right_dir = glm::normalize(glm::cross(m_forward_dir, m_up));
  glm::mat4 rotation_yaw_mat = glm::rotate(glm::mat4(1.0f), yaw_offset, glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 rotation_pitch_mat = glm::rotate(glm::mat4(1.0f), pitch_offset, right_dir);
  m_direction = glm::vec3(rotation_pitch_mat * rotation_yaw_mat * glm::vec4(m_direction, 1.0f));

  // forward dir. of movement unaffected by vertical angle (sticks camera to ground)
  m_forward_dir = {m_direction.x, 0.0f, m_direction.z};
}
