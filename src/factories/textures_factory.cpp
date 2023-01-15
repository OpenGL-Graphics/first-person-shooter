#include <iostream>

#include "factories/textures_factory.hpp"

/* Similar to how programs are managed in <imgui-paint>/Canvas */
TexturesFactory::TexturesFactory():
  // 3D cube texture for skybox (left-handed coords system for cubemaps)
  // See faces order: https://www.khronos.org/opengl/wiki/Cubemap_Texture
  // cubemap images have their origin at upper-left corner (=> don't flip)
  // https://stackoverflow.com/a/11690553/2228912
  m_skybox_images({
    Image("assets/images/skybox/posx.jpg", false), // pos-x (right face)
    Image("assets/images/skybox/negx.jpg", false), // neg-x (left face)
    Image("assets/images/skybox/posy.jpg", false), // pos-y (top face)
    Image("assets/images/skybox/negy.jpg", false), // neg-y (bottom face)
    Image("assets/images/skybox/posz.jpg", false), // pos-z (front face)
    Image("assets/images/skybox/negz.jpg", false)  // neg-z (back face)
  }),

  // smart pointer freed when out of scopt (doesn't work if inserted directly into m_textures)
  m_skybox(std::make_unique<Texture3D>(m_skybox_images)),
  m_health(std::make_unique<Texture2D>(Image("assets/images/surfaces/health.png"))),
  m_wave(std::make_unique<Texture2D>(Image("assets/images/plane/wave.png"))),
  m_cylinder_diffuse(std::make_unique<Texture2D>(Image("assets/images/level/wall_diffuse.jpg"), GL_TEXTURE0)),
  m_cylinder_normal(std::make_unique<Texture2D>(Image("assets/images/level/wall_normal.jpg"), GL_TEXTURE1)),
  m_crosshair(std::make_unique<Texture2D>(Image("assets/images/surfaces/crosshair.png"))),

  m_textures {
    { "skybox", m_skybox.get() },

    // 2D textures for HUDS
    { "health", m_health.get() },

    // 2D texture for flat grid plane (shape made as a sin wave in vertex shader)
    { "wave", m_wave.get() },

    // texture for cylinder pillar
    { "cylinder_diffuse", m_cylinder_diffuse.get() },
    { "cylinder_normal", m_cylinder_normal.get() },

    { "crosshair", m_crosshair.get() },
  }
{
}

/**
 * Templates used to return a Texture2D/3D (abstract class Texture not accepted by std::variant)
 * ::operator[] too tricky to set-up with templates
 */
template <typename T>
T TexturesFactory::get(const std::string& key) {
  // cast Texture (base class) pointer as a Texture2D/3D (derived class) pointer: downcasting
  Texture* texture_base = m_textures[key];
  T* texture_derived = static_cast<T*>(texture_base); // downcast possible else use below

  return *texture_derived;
}

void TexturesFactory::free() {
  for (auto& pair: m_textures) {
    pair.second->free();
  }
}

// explicit template instantition to avoid linking error
template Texture2D TexturesFactory::get<Texture2D>(const std::string& key);
template Texture3D TexturesFactory::get<Texture3D>(const std::string& key);
