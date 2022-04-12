#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <algorithm>
#include <iostream>

#include "render/level_renderer.hpp"
#include "geometries/surface.hpp"
#include "globals/targets.hpp"

/**
 * Sets positions of walls tiles only once in constructor
 * Needed for collision with camera
 */
LevelRenderer::LevelRenderer(const Program& program_tile, const Tilemap& tilemap, const glm::vec3& position, Assimp::Importer& importer):
  m_height(3.5f),
  m_tilemap(tilemap),

  // renderers for walls/floors, props (trees)
  m_renderer_wall(program_tile, VBO(Surface(glm::vec2(1.0f, m_height))), {
    {0, "position", 2, 7, 0},
    {1, "texture_coord", 2, 7, 2},
    {2, "normal", 3, 7, 4},
  }),
  m_renderer_floor(program_tile, VBO(Surface(glm::vec2(m_tilemap.n_cols - 1, m_tilemap.n_rows - 1))), {
    {0, "position", 2, 7, 0},
    {1, "texture_coord", 2, 7, 2},
    {2, "normal", 3, 7, 4},
  }),
  // doesn't have a texture (only a color attached to each mesh in `AssimpUtil::Model::set_mesh_color()`)
  m_tree(importer, "assets/models/tree/tree.obj", Program("assets/shaders/basic.vert", "assets/shaders/basic.frag"), {
    {0, "position", 3, 8, 0},
  }),

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
          glm::vec3 position_target = position_tile + glm::vec3(0.0f, 0.5f, 0.0f);
          Targets::add(Target(position_target));
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

  // TODO: precalculate position of non-moving target cube in constructor
}

/**
 * Render tiles with textures accord. to tilemap & origin at tilemap's upper-left corner
 * @param uniforms Uniforms passed to shader
 */
void LevelRenderer::draw(const Uniforms& u) {
  Uniforms uniforms = u;

  // draw floor & ceiling & targets
  draw_floor(uniforms);
  draw_ceiling(uniforms);
  draw_targets(uniforms);

  for (size_t i_row = 0; i_row < m_tilemap.n_rows; ++i_row) {
    for (size_t i_col = 0; i_col < m_tilemap.n_cols; ++i_col) {
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
          break;
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

/* Draw targets */
void LevelRenderer::draw_targets(const Uniforms& u) {
  for (Target& target : Targets::cubes) {
    target.set_transform(m_transformation);
    target.draw(u);
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
}

/* Renderer lifecycle managed internally */
void LevelRenderer::free() {
  m_renderer_wall.free();
  m_renderer_floor.free();
  m_tree.free();
  Targets::free();

  for (const auto& item : m_textures) {
    item.second.free();
  }
}
