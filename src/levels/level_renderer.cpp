#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <algorithm>
#include <iostream>

#include "levels/level_renderer.hpp"

#include "geometries/surface.hpp"
#include "geometries/cube.hpp"
#include "globals/targets.hpp"

/**
 * Sets positions of object tiles only once in constructor (origin at tilemap's upper-left corner)
 * Needed for collision with camera
 */
LevelRenderer::LevelRenderer(Assimp::Importer& importer, const ShadersFactory& shaders_factory, const TexturesFactory& textures_factory):
  m_tilemap("assets/levels/map.txt"),

  // renderers for props
  m_renderer_doors(shaders_factory, textures_factory),
  m_renderer_floors(shaders_factory, textures_factory, { m_tilemap.n_cols - 1, m_tilemap.n_rows - 1 }),
  m_renderer_walls(shaders_factory, textures_factory),
  m_renderer_windows(shaders_factory, textures_factory),
  m_renderer_trees(shaders_factory, textures_factory, importer),
  m_renderer_targets(shaders_factory, importer),

  m_position(0, 0, 0)
{
  parse_tilemap();
  calculate_uniforms();
}

/* Only calculate world positions & angles in constructor (not in `draw()`) */
void LevelRenderer::parse_tilemap() {
  std::cout << "Tilemap: " << m_tilemap.n_rows << " rows x " << m_tilemap.n_cols << " cols" << '\n';

  std::vector<Tilemap::Tiles> tiles_walls = {
    Tilemap::Tiles::WALL_H, Tilemap::Tiles::WALL_V, Tilemap::Tiles::WALL_L, Tilemap::Tiles::WALL_GAMMA,
  };

  // save position of tile objects (used in `draw()`)
  for (size_t i_row = 0; i_row < m_tilemap.n_rows; ++i_row) {
    for (size_t i_col = 0; i_col < m_tilemap.n_cols; ++i_col) {
      Tilemap::Tiles tile = (Tilemap::Tiles) m_tilemap.map[i_row][i_col];
      glm::vec3 position_tile = {i_col, 0, i_row};
      position_tile += m_position;

      float angle = 0.0f;
      TargetEntry target_entry;
      switch (tile) {
        case Tilemap::Tiles::WALL_H:
          m_walls.push_back({ position_tile, WallOrientation::HORIZONTAL });
          break;
        case Tilemap::Tiles::WALL_V:
          m_walls.push_back({ position_tile, WallOrientation::VERTICAL });
          angle = glm::radians(90.0f);
          break;
        case Tilemap::Tiles::WALL_L:
          m_walls.push_back({ position_tile, WallOrientation::L_SHAPED });
          angle = glm::radians(90.0f);
          break;
        case Tilemap::Tiles::WALL_GAMMA:
          m_walls.push_back({ position_tile, WallOrientation::GAMMA_SHAPED });
          angle = glm::radians(-90.0f);
          break;
        case Tilemap::Tiles::ENEMMY: // non-mobile enemies
          // world-space bbox calculated from `m_targets` local-space bbox in `set_transform()`
          target_entry = { false, position_tile };
          targets.push_back(target_entry);
          continue;
        case Tilemap::Tiles::DOOR_H:
          m_positions_doors.push_back(position_tile);
          break;
        case Tilemap::Tiles::WINDOW:
          m_positions_windows.push_back(position_tile);
          break;
        case Tilemap::Tiles::TREE:
          m_positions_trees.push_back(position_tile);
          break;
        default:
          break;
      } // END CASE

      // save world position for walls (for collision with camera)
      if (std::find(tiles_walls.begin(), tiles_walls.end(), tile) != tiles_walls.end()) {
        glm::vec3 center_local = {0.5f, 0.5f, 0.0f};
        glm::mat4 model = glm::scale(
          glm::rotate(
            glm::translate(glm::mat4(1.0f), position_tile),
            angle,
            glm::vec3(0.0f, 1.0f, 0.0f)
          ),
          glm::vec3(1.0f, m_height, 1.0f)
        );
        glm::vec3 center_world = glm::vec3(model * glm::vec4(center_local, 1.0f));
        positions_walls.push_back(center_world);
      }
    } // END TILEMAP COL
  } // END TILEMAP ROW
}

/**
 * Pass positions parsed from tilemap to renderers classes
 * Called from ctor (after parsing tilemap) to avoid inverting matrices in each frame
 */
void LevelRenderer::calculate_uniforms() {
  // set uniforms matrixes
  m_renderer_targets.calculate_uniforms();
  m_renderer_doors.calculate_uniforms(m_positions_doors);
  m_renderer_walls.calculate_uniforms(m_walls, m_positions_windows);
  m_renderer_trees.calculate_uniforms(m_positions_trees);
  m_renderer_windows.calculate_uniforms(m_positions_windows);
}

/**
 * Set model matrix (translation/rotation/scaling) used by renderers in `draw()`
 * `m_position` serves as an offset when translating surfaces tiles in `draw()`
 * @param frustum Used to avoid drawing objects outside frustum (optimize fps)
 */
void LevelRenderer::set_transform(const Transformation& t, const Frustum& frustum) {
  // set positions of props in appropriate classes
  // Support frustum culling (by filtering out models mats outside frustum)
  m_renderer_targets.set_transform(t, frustum);
  m_renderer_floors.set_transform(t);
  m_renderer_walls.set_transform(t, frustum);
  m_renderer_doors.set_transform(t, frustum);
  m_renderer_trees.set_transform(t, frustum);
  m_renderer_windows.set_transform(t, frustum);
}

/**
 * Render floor, ceiling, doors, & targets (positions parsed in constructor)
 * @param uniforms Uniforms passed to shader (i.e. lights & camera pos.)
 */
void LevelRenderer::draw(const Uniforms& u) {
  m_renderer_targets.draw(u);
  m_renderer_doors.draw(u);
  m_renderer_floors.draw(u);
  m_renderer_trees.draw(u);

  // draw full walls & two walls below/above them
  m_renderer_walls.draw();
  m_renderer_walls.draw_walls_around_window();

  // called last for blending transparent window with objects in bg
  m_renderer_windows.draw();
}

/* Renderers lifecycle managed by other classes */
void LevelRenderer::free() {
  m_renderer_targets.free();
  m_renderer_doors.free();
  m_renderer_floors.free();
  m_renderer_walls.free();
  m_renderer_trees.free();
  m_renderer_windows.free();
}
