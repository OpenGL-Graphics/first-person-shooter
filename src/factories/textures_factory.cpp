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

  // smart pointer freed when out of scopt (doesn't work if inserted directly into m_textures bcos it copies a unique_ptr)
  m_skybox(std::make_unique<Texture3D>(m_skybox_images)),
  m_crosshair(std::make_unique<Texture2D>(Image("assets/images/surfaces/crosshair.png"))),
  m_health(std::make_unique<Texture2D>(Image("assets/images/surfaces/health.png"))),
  m_wall_diffuse(std::make_unique<Texture2D>(Image("assets/images/level/wall_diffuse.jpg"), GL_TEXTURE0)),
  m_wall_normal(std::make_unique<Texture2D>(Image("assets/images/level/wall_normal.jpg"), GL_TEXTURE1)),
  m_door_diffuse(std::make_unique<Texture2D>(Image("assets/images/level/door_diffuse.jpg"), GL_TEXTURE0)),
  m_door_normal(std::make_unique<Texture2D>(Image("assets/images/level/door_normal.jpg"), GL_TEXTURE1)),
  m_window(std::make_unique<Texture2D>(Image("assets/images/surfaces/window.png"))),

  // different texture units as floor/ceiling rendered in same shader by instancing
  m_floor_diffuse(std::make_unique<Texture2D>(Image("assets/images/level/floor_diffuse.jpg"), GL_TEXTURE0)),
  m_floor_normal(std::make_unique<Texture2D>(Image("assets/images/level/floor_normal.jpg"), GL_TEXTURE1)),
  m_ceiling_diffuse(std::make_unique<Texture2D>(Image("assets/images/level/ceiling_diffuse.jpg"), GL_TEXTURE2)),
  m_ceiling_normal(std::make_unique<Texture2D>(Image("assets/images/level/ceiling_normal.jpg"), GL_TEXTURE3)),

  m_textures {
    { "skybox", m_skybox.get() },

    // 2D textures for HUDS
    { "crosshair", m_crosshair.get() },
    { "health", m_health.get() },

    // textures used in LevelRenderer (incl. FloorsRenderer, WallsRenderer)
    { "window", m_window.get() },
    { "door_diffuse", m_door_diffuse.get() },
    { "door_normal", m_door_normal.get() },
    { "floor_diffuse", m_floor_diffuse.get() },
    { "floor_normal", m_floor_normal.get() },
    { "ceiling_diffuse", m_ceiling_diffuse.get() },
    { "ceiling_normal", m_ceiling_normal.get() },
    { "wall_diffuse", m_wall_diffuse.get() },
    { "wall_normal", m_wall_normal.get() },
  }
{
}

/**
 * Templates used to return a Texture2D/3D (abstract class Texture not accepted by std::variant)
 * ::operator[] too tricky to set-up with templates
 */
template <typename T>
T TexturesFactory::get(const std::string& key) const {
  // cast Texture (base class) pointer as a Texture2D/3D (derived class) pointer: downcasting
  Texture* texture_base = m_textures.at(key);
  T* texture_derived = static_cast<T*>(texture_base); // downcast possible else use below

  return *texture_derived;
}

void TexturesFactory::free() {
  for (auto& pair: m_textures) {
    pair.second->free();
  }
}

// explicit template instantition to avoid linking error
template Texture2D TexturesFactory::get<Texture2D>(const std::string& key) const;
template Texture3D TexturesFactory::get<Texture3D>(const std::string& key) const;
