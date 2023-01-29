#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "factories/shaders_factory.hpp"
#include "factories/textures_factory.hpp"
#include "render/renderer.hpp"

/* Window 2D sprite having an image as a texture */
class WindowsRenderer {
public:
  WindowsRenderer(const ShadersFactory& shaders_factory, const TexturesFactory& textures_factory);
  void calculate_uniforms(const std::vector<glm::vec3>& positions);
  void set_transform(const Transformation& t);
  void draw();
  void free();

private:
  const float m_height = 3.5;
  const float height_window = 1.0f;

  Texture2D m_texture;
  Renderer m_renderer;

  std::vector<glm::mat4> m_models;
};

#endif // SPRITE_HPP
