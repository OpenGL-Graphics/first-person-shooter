#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <algorithm>
#include <iostream>

#include "render/level_renderer.hpp"

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
  m_tex_door_diffuse(textures_factory.get<Texture2D>("door_diffuse")),
  m_tex_door_normal(textures_factory.get<Texture2D>("door_normal")),
  m_tex_window(textures_factory.get<Texture2D>("window")),

  // renderers for doors/floors
  m_doors(shaders_factory["tile"], Surface(glm::vec2(1, m_height)), Attributes::get({"position", "normal", "texture_coord"}, 7, true)),
  m_renderer_floors(textures_factory, shaders_factory["tile"], { m_tilemap.n_cols - 1, m_tilemap.n_rows - 1 }),
  m_renderer_walls(textures_factory, shaders_factory["texture_cube"]),

  // tree props don't have a texture (only a color attached to each mesh in `AssimpUtil::Model::set_mesh_color()`)
  m_trees(shaders_factory["texture"], AssimpUtil::Model("assets/models/tree/tree.obj", importer), Attributes::get({"position", "normal", "texture_coord", "tangent"})),

  // window
  m_windows(m_tex_window, shaders_factory["texture_surface"]),

  // target (enemy) to shoot
  m_targets(importer, shaders_factory["texture"]),

  m_position(0, 0, 0)
{
  // TODO: only calculate world positions & angles in constructor (not in `draw()`)
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
 * Render floor, ceiling, doors, & targets (positions parsed in constructor)
 * @param uniforms Uniforms passed to shader (i.e. lights & camera pos.)
 */
void LevelRenderer::draw(const Uniforms& u) {
  draw_doors(u);
  draw_targets(u);
  draw_trees(u);
  m_renderer_walls.draw(m_walls);
  m_renderer_floors.draw(u);

  // called last for blending transparent window with objects in bg
  draw_windows(u);
}

/**
 * Draw doors at locations parsed in constructor
 * Supports instancing
 */
void LevelRenderer::draw_doors(const Uniforms& u) {
  const unsigned int N_DOORS = m_positions_doors.size();
  std::vector<glm::mat4> models_doors(N_DOORS), normals_mats_doors(N_DOORS);
  std::vector<Texture2D> textures_diffuse(N_DOORS), textures_normal(N_DOORS);

  for (size_t i_door = 0; i_door < N_DOORS; ++i_door) {
    glm::vec3 position_door = m_positions_doors[i_door];
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position_door);
    glm::mat4 normal_mat = glm::inverseTranspose(model);
    models_doors[i_door] = model;
    normals_mats_doors[i_door] = normal_mat;

    textures_diffuse[i_door] = m_tex_door_diffuse;
    textures_normal[i_door] = m_tex_door_normal;
  }

  // with face culling enabled, one face (back) of doors surfaces not rendered
  m_doors.set_transform({ models_doors, m_transformation.view, m_transformation.projection });
  m_doors.set_uniform_arr("normals_mats", normals_mats_doors);
  m_doors.set_uniform_arr("textures_diffuse", textures_diffuse);
  m_doors.set_uniform_arr("textures_normal", textures_normal);
  glDisable(GL_CULL_FACE);
  m_doors.draw(u);
  glEnable(GL_CULL_FACE);
}

/**
 * Draw trees 3d model at positions parsed in constructor
 * Supports instancing
 */
void LevelRenderer::draw_trees(const Uniforms& u) {
  const unsigned int N_TREES = m_positions_trees.size();
  std::vector<glm::mat4> models_trees(N_TREES), normals_mats_trees(N_TREES);

  for (size_t i_tree = 0; i_tree < N_TREES; ++i_tree) {
    glm::vec3 position = m_positions_trees[i_tree];
    glm::mat4 model_tree = glm::translate(glm::mat4(1.0f), position);
    models_trees[i_tree] = model_tree;
    normals_mats_trees[i_tree] = glm::inverseTranspose(model_tree);
  }

  m_trees.set_transform({ models_trees, m_transformation.view, m_transformation.projection });
  m_trees.set_uniform_arr("normals_mats", normals_mats_trees);
  m_trees.draw(u);
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

    // draw two walls below & above window (supports instancing)
    m_renderer_walls.draw_walls_around_window(position_tile);
  }

  // with face culling enabled, one face (back) of windows surfaces not rendered
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

  // drawing floors/walls delegated to another class
  m_renderer_floors.set_transform(t);
  m_renderer_walls.set_transform(t);
}

/* Renderer lifecycle managed internally */
void LevelRenderer::free() {
  // renderers
  m_doors.free();
  m_renderer_floors.free();
  m_renderer_walls.free();

  // entities
  m_trees.free();
  m_windows.free();
  m_targets.free();
}
