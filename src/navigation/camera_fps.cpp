#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <algorithm>
#include <iostream>

#include "navigation/camera_fps.hpp"

CameraFPS::CameraFPS(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& u):
  Camera(pos, dir, u),
  m_forward_dir(dir),

  is_jumping(false),
  is_falling(false)
{
}

void CameraFPS::zoom(Zoom z) {
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
 * Update: Turns out brute force collision detection wasn't responsible for low FPS (maybe frustum culling was)
 */
bool CameraFPS::is_close_to_boundaries(const glm::vec3& position_future) {
  // return false;

  std::vector<float> distances;
  std::transform(boundaries.begin(), boundaries.end(), std::back_inserter(distances),
      [position_future](const glm::vec3& position_wall) { return glm::length(position_future - position_wall); });
  float min_distance = *std::min_element(distances.begin(), distances.end());

  return min_distance < 1.2f;
}

void CameraFPS::move(Direction d) {
  // move forward/backward & sideways & up/down
  glm::vec3 right_dir = get_right();
  glm::vec3 position_future;

  switch (d) {
    case Direction::FORWARD:
      position_future = position + SPEED_MOVEMENT*m_forward_dir;
      break;
    case Direction::BACKWARD:
      position_future = position - SPEED_MOVEMENT*m_forward_dir;
      break;
    case Direction::RIGHT:
      position_future = position + SPEED_MOVEMENT*right_dir;
      break;
    case Direction::LEFT:
      position_future = position - SPEED_MOVEMENT*right_dir;
      break;
    case Direction::UP:
      position_future = position + SPEED_MOVEMENT*up;
      break;
    case Direction::DOWN:
      position_future = position - SPEED_MOVEMENT*up;
      break;
  }

  // only move camera if not too close to walls
  if (!is_close_to_boundaries(position_future))
    position = position_future;
}

void CameraFPS::rotate(float x_offset, float y_offset) {
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
  glm::vec3 right_dir = get_right();
  glm::mat4 rotation_yaw_mat = glm::rotate(glm::mat4(1.0f), yaw_offset, glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 rotation_pitch_mat = glm::rotate(glm::mat4(1.0f), pitch_offset, right_dir);
  direction = glm::vec3(rotation_pitch_mat * rotation_yaw_mat * glm::vec4(direction, 1.0f));

  // forward dir. of movement unaffected by vertical angle (sticks camera to ground)
  m_forward_dir = {direction.x, 0.0f, direction.z};
}

/**
 * Jump with camera till a certain elevation then start falling
 * Inspired by: https://codereview.stackexchange.com/a/43187
 */
void CameraFPS::jump() {
  // maximum elevation at y = 3.0f
  if (position.y >= MAX_Y) {
    is_jumping = false;
    is_falling = true;
    return;
  }

  position += SPEED_JUMP*up;
}

/* Same rationale as `CameraFPS::jump()` in other direction */
void CameraFPS::fall() {
  // initial camera elevation at y = 2.0f
  if (position.y <= MIN_Y) {
    is_jumping = false;
    is_falling = false;
    return;
  }

  position -= SPEED_FALL*up;
}

/* called in main loop for a continuous jumping/falling */
void CameraFPS::update() {
  if (is_jumping) {
    jump();
  } else if (is_falling) {
    fall();
  }
}
