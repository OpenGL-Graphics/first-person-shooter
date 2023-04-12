#ifndef WINDOWS_RENDERER_HPP
#define WINDOWS_RENDERER_HPP

#include "factories/shaders_factory.hpp"
#include "factories/textures_factory.hpp"
#include "math/bounding_box.hpp"
#include "render/renderer.hpp"
#include "navigation/frustum.hpp"

/* Window 2D sprite having an image as a texture */
class WindowsRenderer {
public:
  WindowsRenderer(const ShadersFactory& shaders_factory, const TexturesFactory& textures_factory);
  void calculate_uniforms(const std::vector<glm::vec3>& positions_tiles);
  void calculate_bboxes(const std::vector<glm::vec3>& positions_tiles);
  void set_transform(const Transformation& t, const Frustum& frustum);
  void draw();
  void free();

private:
  const float m_height = 3.5;
  const glm::vec3 m_size = { 1, 1, 0 };

  Texture2D m_texture;
  Renderer m_renderer;

  std::vector<glm::mat4> m_models;
  std::vector<BoundingBox> m_bboxes;
};

#endif // WINDOWS_RENDERER_HPP
