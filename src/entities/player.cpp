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
    // calculate bounding box from positions in local coords in vbo
    // then update bounding box in world coords from model matrix (avoids incremental translation)
    BoundingBox bounding_box(renderer.vbo.positions);
    bounding_box.transform(m_transformation.model);

    renderers_bounds.push_back(bounding_box.min);
    renderers_bounds.push_back(bounding_box.max);
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
    move_renderer(offset);
  }
  calculate_bounding_box();
}

/**
 * Translate from current position (model matrix) by given offset
 * Note: bounding box is in world coords while vertexes are in local coords,
 *       hence the different transformation matrices.
 */
void Player::move_renderer(const glm::vec3& offset) {
  // translation vector at 4th column of transformation matrix (i.e. model matrix)
  // vertexes in local coords, hence new_position = old_position + offset
  glm::vec3 position = m_transformation.model[3];
  position += offset;
  m_transformation.model = glm::translate(glm::mat4(1.0f), position);

  // translate bbox instead of reclalculating it each time (like `set_transform()`)
  // update bounding box in world coords using offset
  bounding_box.transform(glm::translate(glm::mat4(1.0f), offset));
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
  m_transformation = { glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 2.5f, 10.0f)), t.view, t.projection };
  m_renderer.set_transform(m_transformation);
}

/* Free renderer (vao/vbo buffers), and shader program */
void Player::free() {
  m_renderer.free();
  m_program.free();
}
