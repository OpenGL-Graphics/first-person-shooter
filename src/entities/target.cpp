#include <glm/gtc/matrix_transform.hpp>

#include "entities/target.hpp"
#include "geometries/cube.hpp"
#include "shader_exception.hpp"

/**
 * Targets are generated inside level accord. to tilemap
 * @param position Position extracted from tilemap
 */
Target::Target(Assimp::Importer& importer, const glm::vec3& pos):
  m_program("assets/shaders/texture_mesh.vert", "assets/shaders/texture_mesh.frag"),
  m_model("assets/models/samurai/samurai.obj", importer),
  m_renderer(m_program, m_model, {
    {0, "position", 3, 11, 0},
    {1, "normal", 3, 11, 3},
    {2, "texture_coord", 2, 11, 6},
    {3, "tangent", 3, 11, 8},
  }),

  is_dead(false),
  position(pos)
{
  // vertex or fragment shaders failed to compile
  if (m_program.has_failed()) {
    throw ShaderException();
  }
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
void Target::set_transform(const Transformation& t) {
  // scale-down samurai 3d model
  glm::mat4 model = glm::scale(
    t.model,
    glm::vec3(1 / 2.0f)
  );

  m_transformation = { model, t.view, t.projection };
  m_renderer.set_transform(m_transformation);
  calculate_bounding_box();
}

/* Calculate bounding box from bounding boxes of each mesh renderer */
void Target::calculate_bounding_box() {
  // concatenate local vertexes xyz
  std::vector<glm::vec3> positions;

  for (Renderer& renderer : m_renderer.renderers) {
    std::vector<glm::vec3> positions_renderer = renderer.vbo.positions;
    positions.insert(positions.end(), positions_renderer.begin(), positions_renderer.end());
  }

  // calculate bounding box from positions in local coords in vbo
  // then update bounding box in world coords from model matrix (avoids incremental translation)
  bounding_box = BoundingBox(positions);
  bounding_box.transform(m_transformation.model);
}

/* Free renderer (vao/vbo buffers) & shader program */
void Target::free() {
  m_renderer.free();
  m_program.free();
}
