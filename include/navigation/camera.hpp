// Inspired by https://learnopengl.com/Getting-started/Camera
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <vector>

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

  /* camera position & look at direction vector */
  glm::vec3 position;
  glm::vec3 direction;

  /* Needed to calculate frustum planes normals (by cross-product) */
  glm::vec3 up;

  /* boundaries of level (i.e. position of walls) */
  std::vector<glm::vec3> boundaries;

  /* for a continuous jumping/falling */
  bool is_jumping;
  bool is_falling;

  Camera(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& u);
  glm::mat4 get_view();
  void move(Direction d);
  void rotate(float x_offset, float y_offset);
  void zoom(Zoom z);
  void update();

  glm::vec3 get_right() const;

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

#endif // CAMERA_HPP
