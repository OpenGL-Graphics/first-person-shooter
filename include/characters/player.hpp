#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "render/model_renderer.hpp"
#include "navigation/camera.hpp"

/* Player character */
struct Player : ModelRenderer {
  /* for collision detection */
  BoundingBox bounding_box;

  Player(const Program& program, const Model& model, const std::vector<Attribute>& attributes);
  void move(Direction direction);
  void calculate_bounding_box();
  void orient(const Camera& camera);

private:
  /* defines direction of movement forward */
  glm::vec3 m_forward_dir;
};

#endif // PLAYER_HPP
