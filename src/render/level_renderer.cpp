#include <glm/gtc/matrix_transform.hpp>

#include "render/level_renderer.hpp"
#include "geometries/surface.hpp"

LevelRenderer::LevelRenderer(const Program& program, const Tilemap& tilemap):
  m_renderer(program, VBO(Surface()), {{0, "position", 2, 4, 0}, {0, "texture_coord", 2, 4, 2}}),
  m_tilemap(tilemap),
  m_textures {
    {"wall", Texture2D(Image("assets/images/level/wall.jpg"))},
    {"door", Texture2D(Image("assets/images/level/door.jpg"))},
  }
{
}

/**
 * Render tiles with textures accord. to tilemap & origin at tilemap's upper-left corner
 * @param uniforms Uniforms passed to shader
 */
void LevelRenderer::draw(Uniforms& uniforms) {
  for (size_t i_row = 0; i_row < m_tilemap.n_rows; ++i_row) {
    for (size_t i_col = 0; i_col < m_tilemap.n_cols; ++i_col) {
      Tilemap::Tiles tile = (Tilemap::Tiles) m_tilemap.map[i_row][i_col];
      glm::vec3 position = {i_col, 0, i_row};
      float angle = 0.0f;

      switch (tile) {
        case Tilemap::Tiles::WALL_H:
          uniforms["texture2d"] = m_textures["wall"];
          break;
        case Tilemap::Tiles::WALL_V:
          uniforms["texture2d"] = m_textures["wall"];
          angle = glm::radians(90.0f);
          break;
        case Tilemap::Tiles::DOOR_H:
          uniforms["texture2d"] = m_textures["door"];
          break;
        case Tilemap::Tiles::DOOR_V:
          uniforms["texture2d"] = m_textures["door"];
          angle = glm::radians(90.0f);
          break;
        default:
          continue;
      }

      // rotation around y-axis before translation & render tile surface
      m_renderer.set_transform(glm::rotate(
        glm::translate(glm::mat4(1.0f), position),
        angle,
        glm::vec3(0.0f, 1.0f, 0.0f)
      ));
      m_renderer.draw(uniforms);
    }
  }
}

void LevelRenderer::free() {
  m_renderer.free();
}
