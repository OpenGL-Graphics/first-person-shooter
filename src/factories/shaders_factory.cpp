#include "factories/shaders_factory.hpp"

/* Similar to how programs are managed in <imgui-paint>/Canvas */
ShadersFactory::ShadersFactory():
  m_programs {
    { "texture", Program("assets/shaders/texture_mesh.vert", "assets/shaders/texture_mesh.frag") },
    { "skybox", Program("assets/shaders/skybox.vert", "assets/shaders/skybox.frag") },
    { "text", Program("assets/shaders/instancing/texture_surface.vert", "assets/shaders/texture_text.frag") },
    { "plane", Program("assets/shaders/light_plane.vert", "assets/shaders/light_plane.frag") },
    { "tile", Program("assets/shaders/tile.vert", "assets/shaders/tile.frag") },
    { "texture_cube", Program("assets/shaders/texture_cube.vert", "assets/shaders/texture_cube.frag") },
    { "light_terrain", Program("assets/shaders/light_terrain.vert", "assets/shaders/light_terrain.frag") },

    // instancing (draw same geometry multiple times at once)
    { "phong", Program("assets/shaders/instancing/phong.vert", "assets/shaders/instancing/phong.frag") },
    { "basic", Program("assets/shaders/instancing/basic.vert", "assets/shaders/instancing/basic.frag") },
    { "texture_surface", Program("assets/shaders/instancing/texture_surface.vert", "assets/shaders/instancing/texture_surface.frag") },
  }
{
}

/**
 * Overloaded bracket operator to get program by name (No setter => readonly)
 * unordered_map::operator[] cannot be used inside const method &
 * must return const ref. (not just ref.) for the same reason (const method => const fields)
 */
const Program& ShadersFactory::operator[](const std::string& key) const {
  return m_programs.at(key);
}

bool ShadersFactory::has_failed() {
  // one of the vertex/fragment shaders failed to compile
  for (auto& pair : m_programs) {
    if (pair.second.has_failed()) {
      return true;
    }
  }

  return false;
}

void ShadersFactory::free() {
  for (auto& pair: m_programs) {
    pair.second.free();
  }
}
