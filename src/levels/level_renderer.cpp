#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <algorithm>
#include <iostream>

#include "levels/level_renderer.hpp"

#include "geometries/surface.hpp"
#include "geometries/cube.hpp"

/**
 * Static class members require a declaration in *.cpp (to allocate space for them)
 * TODO: externalize in `globals/`
 */
std::vector<TargetEntry> LevelRenderer::targets;

/**
 * Sets positions of object tiles only once in constructor (origin at tilemap's upper-left corner)
 * Needed for collision with camera
 */
LevelRenderer::LevelRenderer(Assimp::Importer& importer, const ShadersFactory& shaders_factory, const TexturesFactory& textures_factory):
  m_tilemap("assets/levels/map.txt"),

  // textures
  m_tex_window(textures_factory.get<Texture2D>("window")),

  // renderers for doors/floors
  m_renderer_doors(shaders_factory, textures_factory),
  m_renderer_floors(shaders_factory, textures_factory, { m_tilemap.n_cols - 1, m_tilemap.n_rows - 1 }),
  m_renderer_walls(shaders_factory, textures_factory),

  // tree props don't have a texture (only a color attached to each mesh in `AssimpUtil::Model::set_mesh_color()`)
  m_renderer_trees(shaders_factory, textures_factory, importer),

  // window
  m_windows(m_tex_window, shaders_factory["texture_surface"]),

  // target (enemy) to shoot
  m_targets(importer, shaders_factory["texture"]),

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
          LevelRenderer::targets.push_back(target_entry);
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
  m_renderer_doors.calculate_uniforms(m_positions_doors);
  m_renderer_walls.calculate_uniforms(m_walls, m_positions_windows);
  m_renderer_trees.calculate_uniforms(m_positions_trees);
}

/**
 * Set model matrix (translation/rotation/scaling) used by renderers in `draw()`
 * `m_position` serves as an offset when translating surfaces tiles in `draw()`
 * n_instances = 1 in Transformation as there's only one terrain anyway
 */
void LevelRenderer::set_transform(const Transformation& t) {
  m_transformation = t;

  // update bbox to world coords using model matrix
  for (TargetEntry& target_entry : LevelRenderer::targets) {
    glm::mat4 model_target = glm::translate(glm::mat4(1.0f), target_entry.position);
    target_entry.bounding_box = m_targets.bounding_box;
    target_entry.bounding_box.transform(model_target);
  }

  // set positions of props in appropriate classes
  m_renderer_floors.set_transform(t);
  m_renderer_walls.set_transform(t);
  m_renderer_doors.set_transform(t);
  m_renderer_trees.set_transform(t);
}

/**
 * Render floor, ceiling, doors, & targets (positions parsed in constructor)
 * @param uniforms Uniforms passed to shader (i.e. lights & camera pos.)
 */
void LevelRenderer::draw(const Uniforms& u) {
  draw_targets(u);
  m_renderer_doors.draw(u);
  m_renderer_walls.draw();
  m_renderer_floors.draw(u);
  m_renderer_trees.draw(u);

  // called last for blending transparent window with objects in bg
  draw_windows(u);
}

/**
 * Draw window at mid y-coord of `position_tile` with half-walls below & above it
 * Supports instancing
 */
void LevelRenderer::draw_windows(const Uniforms& u) {
  const unsigned int N_WINDOWS = m_positions_windows.size();
  std::vector<glm::mat4> models_windows(N_WINDOWS);

  for (size_t i_window = 0; i_window < N_WINDOWS; ++i_window) {
    glm::vec3 position_tile = m_positions_windows[i_window];
    float height_window = 1.0f;
    float y_window_bottom = m_height/2.0f - height_window/2.0f;
    glm::vec3 position_window(position_tile.x, y_window_bottom, position_tile.z);
    models_windows[i_window] = glm::translate(glm::mat4(1.0f), position_window);
  }

  // draw windows & two walls below/above them
  // with face culling enabled, one face (back) of windows surfaces not rendered
  m_renderer_walls.draw_walls_around_window();
  m_windows.set_transform({ models_windows, m_transformation.view, m_transformation.projection });
  glDisable(GL_CULL_FACE);
  m_windows.draw();
  glEnable(GL_CULL_FACE);
}

/**
 * Draw targets
 * Supports instancing
 */
void LevelRenderer::draw_targets(const Uniforms& u) {
  const unsigned int N_TARGETS = LevelRenderer::targets.size();
  std::vector<glm::mat4> models_targets(N_TARGETS), normals_mats_targets(N_TARGETS);

  for (size_t i_target = 0; i_target < N_TARGETS; ++i_target) {
    TargetEntry target_entry = LevelRenderer::targets[i_target];
    if (target_entry.is_dead)
      continue;

    // TODO: inverting normal in every iteration :/
    glm::mat4 model_target = glm::translate(glm::mat4(1.0f), target_entry.position);
    glm::mat4 normal_mat = glm::inverseTranspose(model_target);
    models_targets[i_target] = model_target;
    normals_mats_targets[i_target] = normal_mat;
  }

  m_targets.set_transform({ models_targets, m_transformation.view, m_transformation.projection });
  m_targets.set_uniform_arr("normals_mats", normals_mats_targets);
  m_targets.draw(u);
}

/* Renderer lifecycle managed internally */
void LevelRenderer::free() {
  // renderers
  m_renderer_doors.free();
  m_renderer_floors.free();
  m_renderer_walls.free();
  m_renderer_trees.free();

  // entities
  m_windows.free();
  m_targets.free();
}
