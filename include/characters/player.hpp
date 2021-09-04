#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "render/model_renderer.hpp"
#include "navigation/camera.hpp"

/* Player character */
struct Player {
  /* for collision detection */
  BoundingBox bounding_box;

  /* delegate drawing to model renderer */
  ModelRenderer* model_renderer;

  Player(ModelRenderer* renderer);
  void move(Direction direction);
  void calculate_bounding_box();
  void orient(const Camera& camera);
  void draw(const Uniforms& uniforms={});

private:
  /* defines direction of movement forward */
  glm::vec3 m_forward_dir;

  bool m_is_dead;
};

#endif // PLAYER_HPP
