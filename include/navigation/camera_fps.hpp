// Inspired by https://learnopengl.com/Getting-started/Camera
#ifndef CAMERA_FPS_HPP
#define CAMERA_FPS_HPP

#include <glm/glm.hpp>
#include <vector>

#include "navigation/camera.hpp"
#include "navigation/direction.hpp"
#include "navigation/zoom.hpp"

struct CameraFPS : public Camera {
  /* boundaries of level (i.e. position of walls) */
  std::vector<glm::vec3> boundaries;

  /* for a continuous jumping/falling */
  bool is_jumping;
  bool is_falling;

  CameraFPS(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& u);
  void move(Direction d);
  void rotate(float x_offset, float y_offset);
  void zoom(Zoom z);
  void update();

private:
  // camera movements constants
  const float SPEED_MOVEMENT = 0.1f;
  const float SENSITIVITY = 0.25e-2;

  // camera jump/fall constants
  const float SPEED_JUMP = 0.075f;
  const float SPEED_FALL = 0.1f;
  const float MIN_Y = 2.0f;
  const float MAX_Y = 3.0f;

  // direction of movement
  glm::vec3 m_forward_dir;

  bool is_close_to_boundaries(const glm::vec3& position_future);

  void jump();
  void fall();
};

#endif // CAMERA_FPS_HPP
