#include <glm/gtc/matrix_transform.hpp>

#include "entities/target.hpp"
#include "geometries/cube.hpp"

/**
 * Targets are generated inside level accord. to tilemap
 * @param position Position extracted from tilemap
 */
Target::Target(Assimp::Importer& importer, const Program& program):
  m_model("assets/models/samurai/samurai.obj", importer),
  m_renderer(program, m_model, Attributes::get({"position", "normal", "texture_coord", "tangent"})),
  is_dead(false)
{
  calculate_bounding_box();
}

/* delegate drawing with OpenGL (buffers & shaders) to renderer */
void Target::draw(const Uniforms& uniforms) {
  if (is_dead) {
    return;
  }

  m_renderer.draw(uniforms);
}

/**
 * Delegate transform to renderer
 * Translate target to position from tilemap
 */
void Target::set_transform(const Transformation<1>& t) {
  // scale-down samurai 3d model
  glm::mat4 model = glm::scale(
    t.models[0],
    glm::vec3(1 / 2.0f)
  );

  m_transformation = { {model}, t.view, t.projection };
  m_renderer.set_transform(m_transformation);
}

/**
 * Calculate bounding box in local space from bounding boxes of each mesh renderer
 * Called in constructor
 */
void Target::calculate_bounding_box() {
  // concatenate local vertexes xyz
  std::vector<glm::vec3> positions;

  for (Renderer& renderer : m_renderer.renderers) {
    std::vector<glm::vec3> positions_renderer = renderer.vbo.positions;
    positions.insert(positions.end(), positions_renderer.begin(), positions_renderer.end());
  }

  // calculate bounding box from positions in local coords in vbo
  bounding_box = BoundingBox(positions);
}

/* Free renderer (vao/vbo buffers) */
void Target::free() {
  m_renderer.free();
}
