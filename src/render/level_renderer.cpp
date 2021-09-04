#include <glm/gtc/matrix_transform.hpp>

#include "render/level_renderer.hpp"
#include "geometries/surface.hpp"

LevelRenderer::LevelRenderer(const Program& program, const Tilemap& tilemap):
  m_renderer(program, VBO(Surface()), {{0, "position", 2, 4, 0}, {0, "texture_coord", 2, 4, 2}}),
  m_tilemap(tilemap),
  m_textures {
    {"wall", Texture2D(Image("assets/images/level/wall.jpg"))},
    {"door", Texture2D(Image("assets/images/level/door.jpg"))},
    {"floor", Texture2D(Image("assets/images/level/floor.jpg"))},
    {"ceiling", Texture2D(Image("assets/images/level/ceiling.jpg"))},
  },
  m_height(5.0f)
{
}

/**
 * Render tiles with textures accord. to tilemap & origin at tilemap's upper-left corner
 * @param uniforms Uniforms passed to shader
 */
void LevelRenderer::draw(const Uniforms& u) {
  // draw floor & ceiling
  Uniforms uniforms = u;
  draw_floor(uniforms);
  draw_ceiling(uniforms);

  for (size_t i_row = 0; i_row < m_tilemap.n_rows; ++i_row) {
    for (size_t i_col = 0; i_col < m_tilemap.n_cols; ++i_col) {
      Tilemap::Tiles tile = (Tilemap::Tiles) m_tilemap.map[i_row][i_col];
      glm::vec3 position_surface = {i_col, 0, i_row};
      position_surface += m_position;
      float angle[2] = {0.0f, 0.0f};
      unsigned int n_surfaces = 1;

      // choose angle/texture of surface accord. to tile
      switch (tile) {
        case Tilemap::Tiles::WALL_H:
          uniforms["texture2d"] = m_textures["wall"];
          break;
        case Tilemap::Tiles::WALL_V:
          uniforms["texture2d"] = m_textures["wall"];
          angle[0] = glm::radians(90.0f);
          break;
        case Tilemap::Tiles::WALL_L:
          // close gap in lower-left corner using two perpendicular surfaces
          uniforms["texture2d"] = m_textures["wall"];
          n_surfaces = 2;
          angle[0] = glm::radians(90.0f);
          break;
        case Tilemap::Tiles::WALL_L_INV:
          // two perpendicular surfaces forming a reverse L-shape (Uppercase Gamma)
          uniforms["texture2d"] = m_textures["wall"];
          n_surfaces = 2;
          angle[0] = glm::radians(-90.0f);
          break;
        case Tilemap::Tiles::DOOR_H:
          uniforms["texture2d"] = m_textures["door"];
          break;
        case Tilemap::Tiles::DOOR_V:
          uniforms["texture2d"] = m_textures["door"];
          angle[0] = glm::radians(90.0f);
          break;
        case Tilemap::Tiles::SPACE:
          continue;
      }

      // render tile surface (or two tiles surfaces for corners)
      for (size_t i_surface = 0; i_surface < n_surfaces; ++i_surface) {
        // vertical scaling then rotation around y-axis then translation
        m_renderer.set_transform({
          glm::scale(
            glm::rotate(
              glm::translate(glm::mat4(1.0f), position_surface),
              angle[i_surface],
              glm::vec3(0.0f, 1.0f, 0.0f)
            ),
            glm::vec3(1.0f, m_height, 1.0f)
          ), m_renderer.transformation.view, m_renderer.transformation.projection
        });

        m_renderer.draw(uniforms);
      }
    }
  }
}

/**
 * Draw horizontal surface at given height
 * @param size Amount by which to scale surface on xy vertical plan
 * @param height Elevation of surface (i.e. translation on y-axis)
 */
void LevelRenderer::draw_horizontal_surface(const Uniforms& uniforms, const glm::vec2& size, float height) {
  // xy scaling then rotation around x-axis then translation
  float angle = glm::radians(90.0f);
  m_renderer.set_transform({
    glm::scale(
      glm::rotate(
        glm::translate(glm::mat4(1.0f), m_position + glm::vec3(0.0f, height, 0.0f)),
        angle,
        glm::vec3(1.0f, 0.0f, 0.0f)
      ),
      glm::vec3(size.x, size.y, 1.0f)
    ), m_renderer.transformation.view, m_renderer.transformation.projection
  });

  m_renderer.draw(uniforms);
}

/* Draw horizontal floor covering bottom of tilemap */
void LevelRenderer::draw_floor(const Uniforms& u) {
  Uniforms uniforms = u;
  uniforms["texture2d"] = m_textures["floor"];
  draw_horizontal_surface(uniforms, {m_tilemap.n_cols - 1, m_tilemap.n_rows - 1}, 0.0f);
}

/* Draw horizontal floor covering top of tilemap */
void LevelRenderer::draw_ceiling(const Uniforms& u) {
  Uniforms uniforms = u;
  uniforms["texture2d"] = m_textures["ceiling"];
  draw_horizontal_surface(uniforms, {m_tilemap.n_cols - 1, m_tilemap.n_rows - 1}, m_height);
}

/**
 * Set model matrix (translation/rotation/scaling)
 * `m_position` serves as an offset when translating surfaces tiles in `draw()`
 */
void LevelRenderer::set_transform(const Transformation& t) {
  m_renderer.set_transform(t);
  m_position = m_renderer.model_mat[3];
}

void LevelRenderer::free() {
  m_renderer.free();
}
