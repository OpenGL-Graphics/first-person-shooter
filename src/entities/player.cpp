#include <glm/gtc/matrix_transform.hpp>

#include "entities/player.hpp"

// not declared as private members as constants cause class's implicit copy-constructor to be deleted (prevents re-assignment)
// movement constants
const float SPEED = 0.1f;

Player::Player(Assimp::Importer& importer):
  m_program("assets/shaders/texture_mesh.vert", "assets/shaders/texture_mesh.frag"),
  m_model("assets/models/cube-textured/cube-textured.obj", importer),
  m_renderer(m_program, m_model, {{0, "position", 3, 11, 0}, {1, "normal", 3, 11, 3}, {2, "texture_coord", 2, 11, 6}}),
  m_forward_dir(0.0f, 0.0f, -1.0f),
  m_is_dead(false)
{
}

/* Calculate bounding box from bounding boxes of each mesh renderer */
void Player::calculate_bounding_box() {
  std::vector<glm::vec3> renderers_bounds;
  for (Renderer& renderer : m_renderer.renderers) {
    renderers_bounds.push_back(renderer.bounding_box.min);
    renderers_bounds.push_back(renderer.bounding_box.max);
  }

  bounding_box = BoundingBox(renderers_bounds);
}

/* Move 3D model in either of the four directions accord to given `direction` */
void Player::move(Direction direction) {
  glm::vec3 offset;
  glm::vec3 up_dir(0.0f, 1.0f, 0.0f);

  if (direction == Direction::FORWARD) {
    offset = SPEED * m_forward_dir;
  }
  if (direction == Direction::BACKWARD) {
    offset = SPEED * -m_forward_dir;
  }
  if (direction == Direction::LEFT) {
    glm::vec3 left_dir = glm::normalize(glm::cross(up_dir, m_forward_dir));
    offset = SPEED * left_dir;
  }
  if (direction == Direction::RIGHT) {
    glm::vec3 right_dir = glm::normalize(glm::cross(m_forward_dir, up_dir));
    offset = SPEED * right_dir;
  }

  // move meshes & recalculate model's bounding box
  for (Renderer& renderer : m_renderer.renderers) {
    renderer.move(offset);
  }
  calculate_bounding_box();
}

/**
 * Adjust player movements according to camera's orientation
 * i.e. to yaw angle relative to vertical y-axis
 */
void Player::orient(const Camera& camera) {
    // negated yaw as pc's look angle follows camera rotation (i.e. inverse of scene's view matrix)
    float yaw_camera = -camera.yaw;

    // update pc's forward movement direction accord. to camera's yaw angle (around y-axis)
    if (yaw_camera >= glm::radians(-45.0f) && yaw_camera <= glm::radians(45.0f)) {
      m_forward_dir = {0.0f, 0.0f, -1.0f};
    } else if (yaw_camera <= glm::radians(-45.0f)) {
      m_forward_dir = {1.0f, 0.0f, 0.0f};
    } else if (yaw_camera >= glm::radians(45.0f)) {
      m_forward_dir = {-1.0f, 0.0f, 0.0f};
    }
}

/* delegate drawing with OpenGL (buffers & shaders) to renderer */
void Player::draw(const Uniforms& uniforms) {
  if (m_is_dead) {
    return;
  }

  m_renderer.draw(uniforms);
}

void Player::set_transform(const Transformation& t) {
  m_renderer.set_transform({ glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 2.5f, 10.0f)), t.view, t.projection });
}

/* Free renderer (vao/vbo buffers), and shader program */
void Player::free() {
  m_renderer.free();
  m_program.free();
}
