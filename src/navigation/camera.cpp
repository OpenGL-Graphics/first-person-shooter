#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <algorithm>
#include <iostream>

#include "navigation/camera.hpp"

// not declared as private members as constants cause class's implicit copy-constructor to be deleted (prevents re-assignment)
// movement constants
const float SPEED = 0.1f;
const float SENSITIVITY = 0.25e-2;

Camera::Camera(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& up):
  position(pos),
  direction(dir),
  m_up(up),
  m_forward_dir(dir),

  pitch(0.0f),
  yaw(0.0f),
  fov(45.0f)
{
}

/**
 * View matrix orients a scene in such a way to simulate camera's movement (inversion of camera's transformation matrix)
 * i.e. translation/rotation of scene in opposite directions to camera
 * Important: pitch/yaw angles rotate scene's object not camera
 */
glm::mat4 Camera::get_view() {
  // only camera direction is rotated by mouse in `rotate()`
  glm::mat4 view = glm::lookAt(position, position + direction, m_up);

  return view;
}

void Camera::zoom(Zoom z) {
  if (z == Zoom::IN) {
    fov -= 5.0f;
  } else if (z == Zoom::OUT) {
    fov += 5.0f;
  }
}

/**
 * Check if future camera position is too close from its distance to closest wall tile
 * Used to prevent camera from going through walls
 * @param position_future Next position of camera
 */
bool Camera::is_close_to_boundaries(const glm::vec3& position_future) {
  std::vector<float> distances;
  std::transform(boundaries.begin(), boundaries.end(), std::back_inserter(distances),
      [position_future](const glm::vec3& position_wall) { return glm::length(position_future - position_wall); });
  float min_distance = *std::min_element(distances.begin(), distances.end());

  return min_distance < 1.2f;
}

void Camera::move(Direction d) {
  // move forward/backward & sideways
  glm::vec3 right_dir = glm::normalize(glm::cross(m_forward_dir, m_up));
  glm::vec3 position_future;

  if (d == Direction::FORWARD) {
    position_future = position + SPEED*m_forward_dir;
  } else if (d == Direction::BACKWARD) {
    position_future = position - SPEED*m_forward_dir;
  } else if (d == Direction::RIGHT) {
    position_future = position + SPEED*right_dir;
  } else if (d == Direction::LEFT) {
    position_future = position - SPEED*right_dir;
  }

  // only move camera if not too close to walls
  std::vector<Direction> dirs = {
    Direction::FORWARD, Direction::BACKWARD, Direction::RIGHT, Direction::LEFT};
  if (std::find(dirs.begin(), dirs.end(), d) != dirs.end() && !is_close_to_boundaries(position_future))
    position = position_future;
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
  direction = glm::vec3(rotation_pitch_mat * rotation_yaw_mat * glm::vec4(direction, 1.0f));

  // forward dir. of movement unaffected by vertical angle (sticks camera to ground)
  m_forward_dir = {direction.x, 0.0f, direction.z};
}
