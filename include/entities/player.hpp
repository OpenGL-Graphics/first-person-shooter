#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "render/model_renderer.hpp"
#include "navigation/camera.hpp"

/* Player character */
struct Player {
  /* for collision detection */
  BoundingBox bounding_box;

  Player(Assimp::Importer& importer);
  void move(Direction direction);
  void calculate_bounding_box();
  void orient(const Camera& camera);
  void draw(const Uniforms& uniforms={});
  void set_transform(const Transformation& t);
  void free();

private:
  /* defines direction of movement forward */
  glm::vec3 m_forward_dir;

  bool m_is_dead;

  // program declared first (i.e. init first) as it's needed by renderer
  // https://stackoverflow.com/a/41886567/2228912
  Program m_program;

  /* delegate drawing to model renderer */
  Model m_model;
  ModelRenderer m_renderer;
};

#endif // PLAYER_HPP
