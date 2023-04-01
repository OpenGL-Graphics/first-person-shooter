#include <iostream>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "entries/target_entry.hpp"
#include "levels/targets_renderer.hpp"
#include "globals/targets.hpp"
#include "geometries/cube.hpp"

/**
 * Targets are generated inside level accord. to tilemap
 * @param position Position extracted from tilemap
 */
TargetsRenderer::TargetsRenderer(const ShadersFactory& shaders_factory, Assimp::Importer& importer):
  m_model3d("assets/models/samurai/samurai.obj", importer),
  m_renderer(shaders_factory["texture"], m_model3d, Attributes::get({"position", "normal", "texture_coord", "tangent"}))
{
  calculate_bounding_box();
}

/**
 * Calculate bounding box in local space from bounding boxes of each mesh renderer
 * Called in constructor
 */
void TargetsRenderer::calculate_bounding_box() {
  // concatenate local vertexes xyz
  std::vector<glm::vec3> positions;

  for (Renderer& renderer : m_renderer.renderers) {
    std::vector<glm::vec3> positions_renderer = renderer.vbo.positions;
    positions.insert(positions.end(), positions_renderer.begin(), positions_renderer.end());
  }

  // calculate bounding box from positions in local coords in vbo
  m_bounding_box = BoundingBox(positions);
}

/* Called from LevelRenderer ctor to avoid calculating inverse & bbox each frame */
void TargetsRenderer::calculate_uniforms() {
  const unsigned int N_TARGETS = targets.size();
  m_models.resize(N_TARGETS);
  m_normals_mats.resize(N_TARGETS);

  for (size_t i_target = 0; i_target < N_TARGETS; ++i_target) {
    // scale-down samurai 3d model (scaling then translation)
    TargetEntry& target_entry = targets[i_target];
    glm::mat4 model_target = glm::scale(
      glm::translate(glm::mat4(1.0f), target_entry.position),
      glm::vec3(1 / 2.0f)
    );
    glm::mat4 normal_mat = glm::inverseTranspose(model_target);

    m_models[i_target] = model_target;
    m_normals_mats[i_target] = normal_mat;

    // update global var's bbox to world coords using model matrix
    // needed for mouse intersection in MouseHandler
    target_entry.bounding_box = m_bounding_box;
    target_entry.bounding_box.transform(model_target);
  }
}

/* Keeps in uniform matrices only alive targets to avoid drawing dead ones */
std::vector<glm::mat4> TargetsRenderer::cull_dead(const std::vector<glm::mat4>& matrices, const std::vector<TargetEntry> targets) {
  std::vector<glm::mat4> matrices_out;

  for (size_t i_target = 0; i_target < targets.size(); ++i_target) {
    TargetEntry target_entry = targets[i_target];

    // check if not dead
    if (!target_entry.is_dead) {
      glm::mat4 mat = matrices[i_target];
      matrices_out.push_back(mat);
    }
  }

  return matrices_out;
}

/**
 * Delegate transform to renderer
 * Translate target to position from tilemap
 */
void TargetsRenderer::set_transform(const Transformation& t, const Frustum& frustum) {
  // frustum culling first
  std::vector<glm::vec3> positions;
  auto get_position = [](const auto& target) { return target.position; };
  std::transform(targets.begin(), targets.end(), std::back_inserter(positions), get_position);

  std::vector<glm::mat4> models_frustum = frustum.cull(m_models, positions);
  std::vector<glm::mat4> normals_mats_frustum = frustum.cull(m_normals_mats, positions);
  std::vector<TargetEntry> targets_frustum = frustum.cull(targets, positions);

  // ignore dead targets (to avoid drawing them)
  std::vector<glm::mat4> models = cull_dead(models_frustum, targets_frustum);
  std::vector<glm::mat4> normals_mats = cull_dead(normals_mats_frustum, targets_frustum);

  m_renderer.set_transform({ models, t.view, t.projection });
  m_renderer.set_uniform_arr("normals_mats", normals_mats);
}

/* delegate drawing with OpenGL (buffers & shaders) to renderer */
void TargetsRenderer::draw(const Uniforms& uniforms) {
  // ignore dead targets (to avoid drawing them)
  m_renderer.draw(uniforms);
}

/* Free renderer (vao/vbo buffers) */
void TargetsRenderer::free() {
  m_model3d.free();
  m_renderer.free();
}
