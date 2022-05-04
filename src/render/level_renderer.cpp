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
LevelRenderer::LevelRenderer(const Program& program_tile, const Tilemap& tilemap, const glm::vec3& position, Assimp::Importer& importer):
  m_tilemap(tilemap),

  // renderers for doors/floors
  m_renderer_door(program_tile, VBO(Surface(glm::vec2(1.0f, m_height))), {
    {0, "position", 2, 7, 0},
    {1, "normal", 3, 7, 2},
    {2, "texture_coord", 2, 7, 5},
  }),
  m_renderer_floors(program_tile, { m_tilemap.n_cols - 1, m_tilemap.n_rows - 1 }),
  m_renderer_walls(),

  // tree props don't have a texture (only a color attached to each mesh in `AssimpUtil::Model::set_mesh_color()`)
  m_tree(importer, "assets/models/tree/tree.obj", Program("assets/shaders/texture_mesh.vert", "assets/shaders/texture_mesh.frag"), {
    {0, "position", 3, 11, 0},
    {1, "normal", 3, 11, 3},
    {2, "texture_coord", 2, 11, 6},
    {3, "tangent", 3, 11, 8},
  }),

  // window
  m_window(Image("assets/images/surfaces/window.png")),

  // target (enemy) to shoot
  m_target(importer),

  m_textures {
    {"wall_diffuse", Texture2D(Image("assets/images/level/wall_diffuse.jpg"), GL_TEXTURE0)},
    {"wall_normal", Texture2D(Image("assets/images/level/wall_normal.jpg"), GL_TEXTURE1)},
    {"door_diffuse", Texture2D(Image("assets/images/level/door_diffuse.jpg"), GL_TEXTURE0)},
    {"door_normal", Texture2D(Image("assets/images/level/door_normal.jpg"), GL_TEXTURE1)},
  },

  m_position(position)
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
      }

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
    }
  }
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

/* Draw doors at locations parsed in constructor */
void LevelRenderer::draw_doors(const Uniforms& u) {
  Uniforms uniforms = u;
  uniforms["texture_diffuse"] = m_textures["door_diffuse"];
  uniforms["texture_normal"] = m_textures["door_normal"];

  for (const glm::vec3& position_door : m_positions_doors) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position_door);
    glm::mat4 normal_mat = glm::inverseTranspose(model);
    uniforms["normal_mat"] = normal_mat;

    m_renderer_door.set_transform({ model, m_transformation.view, m_transformation.projection });
    m_renderer_door.draw(uniforms);
  }
}

/* Draw trees 3d model at positions parsed in constructor */
void LevelRenderer::draw_trees(const Uniforms& u) {
  for (const glm::vec3& position : m_positions_trees) {
    // 3d model (origin: base of 3d model)
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    m_tree.set_transform({ model, m_transformation.view, m_transformation.projection });

    Uniforms uniforms = u;
    glm::mat4 normal_mat = glm::inverseTranspose(model);
    uniforms["normal_mat"] = normal_mat;
    m_tree.draw(uniforms);
  }
}

void LevelRenderer::draw_windows(const Uniforms& u) {
  for (const glm::vec3& position: m_positions_windows) {
    draw_window(u, position);
  }
}

/* Draw window at mid y-coord of `position_tile` with half-walls below & above it */
void LevelRenderer::draw_window(const Uniforms& u, const glm::vec3& position_tile) {
  float height_window = 1.0f;
  float z_window_bottom = m_height/2.0f - height_window/2.0f;
  glm::vec3 position_window(position_tile.x, z_window_bottom, position_tile.z);

  // transparent surfaces last to render to ensure blending with background
  m_window.set_transform({
    glm::translate(glm::mat4(1.0f), position_window),
    m_transformation.view, m_transformation.projection
  });
  m_window.draw();

  // draw two walls below & above window
  m_renderer_walls.draw_walls_around_window(position_tile);
}

/* Draw targets */
void LevelRenderer::draw_targets(const Uniforms& u) {
  Uniforms uniforms = u;

  for (TargetEntry& target_entry : LevelRenderer::targets) {
    if (target_entry.is_dead)
      continue;

    glm::mat4 model_target = get_model_target(target_entry.position);
    glm::mat4 normal_mat = glm::inverseTranspose(model_target);
    uniforms["normal_mat"] = normal_mat;
    m_target.set_transform({ model_target, m_transformation.view, m_transformation.projection });
    m_target.draw(uniforms);
  }
}

/**
 * Set model matrix (translation/rotation/scaling) used by renderers in `draw()`
 * `m_position` serves as an offset when translating surfaces tiles in `draw()`
 */
void LevelRenderer::set_transform(const Transformation& t) {
  m_transformation = t;

  // update bbox to world coords using model matrix
  for (TargetEntry& target_entry : LevelRenderer::targets) {
    glm::mat4 model_target = get_model_target(target_entry.position);
    target_entry.bounding_box = m_target.bounding_box;
    target_entry.bounding_box.transform(model_target);
  }

  // drawing floors/walls delegated to another class
  m_renderer_floors.set_transform(t);
  m_renderer_walls.set_transform(t);
}

/* add-up level & target translation offsets */
glm::mat4 LevelRenderer::get_model_target(const glm::vec3& position_target) {
  glm::vec3 position_level = m_transformation.model[3];
  glm::vec3 position = position_level + position_target;
  glm::mat4 model_target = glm::translate(glm::mat4(1.0f), position);

  return model_target;
}

/* Renderer lifecycle managed internally */
void LevelRenderer::free() {
  // renderers
  m_renderer_door.free();
  m_renderer_floors.free();
  m_renderer_walls.free();

  // entities
  m_tree.free();
  m_window.free();
  m_target.free();

  // textures
  for (auto& item : m_textures) {
    item.second.free();
  }
}
