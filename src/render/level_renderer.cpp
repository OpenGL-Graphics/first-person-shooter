#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <algorithm>
#include <iostream>

#include "render/level_renderer.hpp"
#include "geometries/surface.hpp"

/**
 * Static class members require a declaration in *.cpp (to allocate space for them)
 * TODO: externalize in `globals/`
 */
std::vector<TargetEntry> LevelRenderer::targets;

/**
 * Sets positions of walls tiles only once in constructor
 * Needed for collision with camera
 */
LevelRenderer::LevelRenderer(const Program& program_tile, const Tilemap& tilemap, const glm::vec3& position, Assimp::Importer& importer):
  m_height(3.5f),
  m_tilemap(tilemap),

  // renderers for walls/floors
  m_renderer_wall(program_tile, VBO(Surface(glm::vec2(1.0f, m_height))), {
    {0, "position", 2, 7, 0},
    {1, "normal", 3, 7, 2},
    {2, "texture_coord", 2, 7, 5},
  }),
  m_renderer_floor(program_tile, VBO(Surface(glm::vec2(m_tilemap.n_cols - 1, m_tilemap.n_rows - 1))), {
    {0, "position", 2, 7, 0},
    {1, "normal", 3, 7, 2},
    {2, "texture_coord", 2, 7, 5},
  }),

  // tree props don't have a texture (only a color attached to each mesh in `AssimpUtil::Model::set_mesh_color()`)
  m_tree(importer, "assets/models/tree/tree.obj", Program("assets/shaders/basic.vert", "assets/shaders/basic.frag"), {
    {0, "position", 3, 11, 0},
  }),

  // window & wall above/below it
  m_window(Image("assets/images/surfaces/window.png")),
  m_renderer_wall_half(program_tile, VBO(Surface(glm::vec2(1.0f, m_height/2.0f - 1.0f/2.0f))), {
    {0, "position", 2, 7, 0},
    {1, "normal", 3, 7, 2},
    {2, "texture_coord", 2, 7, 5},
  }),

  // target (enemy) to shoot
  m_target(importer),

  m_textures {
    {"wall_diffuse", Texture2D(Image("assets/images/level/wall_diffuse.jpg"), GL_TEXTURE0)},
    {"wall_normal", Texture2D(Image("assets/images/level/wall_normal.jpg"), GL_TEXTURE1)},
    {"door_diffuse", Texture2D(Image("assets/images/level/door_diffuse.jpg"), GL_TEXTURE0)},
    {"door_normal", Texture2D(Image("assets/images/level/door_normal.jpg"), GL_TEXTURE1)},
    {"floor_diffuse", Texture2D(Image("assets/images/level/floor_diffuse.jpg"), GL_TEXTURE0)},
    {"floor_normal", Texture2D(Image("assets/images/level/floor_normal.jpg"), GL_TEXTURE1)},
    {"ceiling_diffuse", Texture2D(Image("assets/images/level/ceiling_diffuse.jpg"), GL_TEXTURE0)},
    {"ceiling_normal", Texture2D(Image("assets/images/level/ceiling_normal.jpg"), GL_TEXTURE1)},
  },
  m_position(position)
{
  // TODO: only calculate world positions & angles in constructor (not in `draw()`)
  std::cout << "rows: " << m_tilemap.n_rows << '\n';
  std::cout << "cols: " << m_tilemap.n_cols << '\n';

  std::vector<Tilemap::Tiles> tiles_walls = {
    Tilemap::Tiles::WALL_H, Tilemap::Tiles::WALL_V, Tilemap::Tiles::WALL_L, Tilemap::Tiles::WALL_L_INV,
  };

  for (size_t i_row = 0; i_row < m_tilemap.n_rows; ++i_row) {
    for (size_t i_col = 0; i_col < m_tilemap.n_cols; ++i_col) {
      Tilemap::Tiles tile = (Tilemap::Tiles) m_tilemap.map[i_row][i_col];
      glm::vec3 position_tile = {i_col, 0, i_row};
      position_tile += m_position;

      float angle = 0.0f;
      switch (tile) {
        case Tilemap::Tiles::WALL_H:
          break;
        case Tilemap::Tiles::WALL_V:
          angle = glm::radians(90.0f);
          break;
        case Tilemap::Tiles::WALL_L:
          angle = glm::radians(90.0f);
          break;
        case Tilemap::Tiles::WALL_L_INV:
          angle = glm::radians(-90.0f);
          break;
        case Tilemap::Tiles::ENEMMY: // non-mobile enemies
          // world-space bbox calculated from `m_targets` local-space bbox in `set_transform()`
          TargetEntry target_entry = { false, position_tile };
          LevelRenderer::targets.push_back(target_entry);
          continue;
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
 * Render tiles with textures accord. to tilemap & origin at tilemap's upper-left corner
 * @param uniforms Uniforms passed to shader
 */
void LevelRenderer::draw(const Uniforms& u) {
  // draw floor & ceiling & targets
  draw_floor(u);
  draw_ceiling(u);
  draw_targets(u);

  for (size_t i_row = 0; i_row < m_tilemap.n_rows; ++i_row) {
    for (size_t i_col = 0; i_col < m_tilemap.n_cols; ++i_col) {
      Uniforms uniforms = u;
      Tilemap::Tiles tile = (Tilemap::Tiles) m_tilemap.map[i_row][i_col];
      glm::vec3 position_tile = {i_col, 0, i_row};
      position_tile += m_position;
      float angle[2] = {0.0f, 0.0f};
      unsigned int n_surfaces = 1;

      // choose angle/texture of surface accord. to tile
      switch (tile) {
        case Tilemap::Tiles::ENEMMY:
          continue;
        case Tilemap::Tiles::TREE:
          // 3d model
          m_tree.set_transform({
            glm::translate(glm::mat4(1.0f), position_tile),  // origin: base of 3d model
            m_transformation.view, m_transformation.projection });
          m_tree.draw(uniforms);
          continue;
        case Tilemap::Tiles::WINDOW:
          draw_window(uniforms, position_tile);
          continue;
        case Tilemap::Tiles::SPACE:
          continue;
        case Tilemap::Tiles::WALL_H:
          uniforms["texture_diffuse"] = m_textures["wall_diffuse"];
          uniforms["texture_normal"] = m_textures["wall_normal"];
          break;
        case Tilemap::Tiles::WALL_V:
          uniforms["texture_diffuse"] = m_textures["wall_diffuse"];
          uniforms["texture_normal"] = m_textures["wall_normal"];
          angle[0] = glm::radians(90.0f);
          break;
        case Tilemap::Tiles::WALL_L:
          // close gap in lower-left corner using two perpendicular surfaces
          uniforms["texture_diffuse"] = m_textures["wall_diffuse"];
          uniforms["texture_normal"] = m_textures["wall_normal"];
          n_surfaces = 2;
          angle[0] = glm::radians(90.0f);
          break;
        case Tilemap::Tiles::WALL_L_INV:
          // two perpendicular surfaces forming a reverse L-shape (Uppercase Gamma)
          uniforms["texture_diffuse"] = m_textures["wall_diffuse"];
          uniforms["texture_normal"] = m_textures["wall_normal"];
          n_surfaces = 2;
          angle[0] = glm::radians(-90.0f);
          break;
        case Tilemap::Tiles::DOOR_H:
          uniforms["texture_diffuse"] = m_textures["door_diffuse"];
          uniforms["texture_normal"] = m_textures["door_normal"];
          break;
        case Tilemap::Tiles::DOOR_V:
          uniforms["texture_diffuse"] = m_textures["door_diffuse"];
          uniforms["texture_normal"] = m_textures["door_normal"];
          angle[0] = glm::radians(90.0f);
          break;
      }

      // render tile surface (or two tiles surfaces for corners): wall or door
      for (size_t i_surface = 0; i_surface < n_surfaces; ++i_surface) {
        // calculate normal matrix only once (instead of doing it in shader for every vertex)
        glm::mat4 model = glm::rotate(
          glm::translate(glm::mat4(1.0f), position_tile),
          angle[i_surface],
          glm::vec3(0.0f, 1.0f, 0.0f)
        );
        glm::mat4 normal_mat = glm::inverseTranspose(model);
        uniforms["normal_mat"] = normal_mat;

        // vertical scaling then rotation around y-axis then translation
        m_renderer_wall.set_transform({ model, m_transformation.view, m_transformation.projection });
        m_renderer_wall.draw(uniforms);
      }
    }
  }
}

/* Draw window at mid y-coord of `position_tile` with half-walls below & above it */
void LevelRenderer::draw_window(const Uniforms& u, const glm::vec3& position_tile) {
  float height_window = 1.0f;
  float z_window_bottom = m_height/2.0f - height_window/2.0f;
  glm::vec3 position_window(position_tile.x, z_window_bottom, position_tile.z);

  // TODO: transparent surfaces should be last to render to ensure blending with background
  m_window.set_transform({
    glm::translate(glm::mat4(1.0f), position_window),
    m_transformation.view, m_transformation.projection
  });
  m_window.draw();

  // wall below window
  Uniforms uniforms = u;
  uniforms["texture_diffuse"] = m_textures["wall_diffuse"];
  uniforms["texture_normal"] = m_textures["wall_normal"];

  glm::mat4 model_top = glm::translate(glm::mat4(1.0f), position_tile);
  uniforms["normal_mat"] = glm::inverseTranspose(model_top);
  m_renderer_wall_half.set_transform({ model_top, m_transformation.view, m_transformation.projection });
  m_renderer_wall_half.draw(uniforms);

  // wall above window
  float z_window_top = z_window_bottom + height_window;
  glm::mat4 model_bottom = glm::translate(glm::mat4(1.0f), glm::vec3(position_tile.x, z_window_top, position_tile.z));
  uniforms["normal_mat"] = glm::inverseTranspose(model_bottom);
  m_renderer_wall_half.set_transform({ model_bottom, m_transformation.view, m_transformation.projection });
  m_renderer_wall_half.draw(uniforms);
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
    m_target.draw(u);
  }
}

/**
 * Draw horizontal surface at given height
 * @param height Elevation of surface (i.e. translation on y-axis)
 */
void LevelRenderer::draw_horizontal_surface(const Uniforms& u, float height) {
  // calculate normal matrix only once (instead of doing it in shader for every vertex)
  float angle = glm::radians(90.0f);
  glm::mat4 model = glm::rotate(
    glm::translate(glm::mat4(1.0f), m_position + glm::vec3(0.0f, height, 0.0f)),
    angle,
    glm::vec3(1.0f, 0.0f, 0.0f)
  );
  glm::mat4 normal_mat = glm::inverseTranspose(model);

  // xy scaling then rotation around x-axis then translation
  Uniforms uniforms = u;
  uniforms["normal_mat"] = normal_mat;
  m_renderer_floor.set_transform({ model, m_transformation.view, m_transformation.projection });
  m_renderer_floor.draw(uniforms);
}

/* Draw horizontal floor covering bottom of tilemap */
void LevelRenderer::draw_floor(const Uniforms& u) {
  Uniforms uniforms = u;
  uniforms["texture_diffuse"] = m_textures["floor_diffuse"];
  uniforms["texture_normal"] = m_textures["floor_normal"];
  draw_horizontal_surface(uniforms, 0.0f);
}

/* Draw horizontal floor covering top of tilemap */
void LevelRenderer::draw_ceiling(const Uniforms& u) {
  Uniforms uniforms = u;
  uniforms["texture_diffuse"] = m_textures["ceiling_diffuse"];
  uniforms["texture_normal"] = m_textures["ceiling_normal"];
  draw_horizontal_surface(uniforms, m_height);
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
  m_renderer_wall.free();
  m_renderer_floor.free();
  m_tree.free();
  m_window.free();
  m_renderer_wall_half.free();
  m_target.free();

  for (const auto& item : m_textures) {
    item.second.free();
  }
}
