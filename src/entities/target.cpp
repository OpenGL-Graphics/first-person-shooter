#include <glm/gtc/matrix_transform.hpp>

#include "entities/target.hpp"
#include "geometries/cube.hpp"
#include "shader_exception.hpp"

/**
 * Targets are generated inside level accord. to tilemap
 * TODO:
 *   1. pass ModelRenderer (like for Player)
 *   2. Both Target & Player inherit from Character class containing draw() & m_is_dead
 * @param position Position extracted from tilemap
 */
Target::Target(const glm::vec3& position):
  m_program("assets/shaders/color.vert", "assets/shaders/color.frag"),
  renderer(m_program, VBO(Cube{}), {{0, "position", 3, 12, 0}, {1, "color", 3, 12, 3}}),  // render colored cube
  is_dead(false),
  m_position(position)
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

  renderer.draw(uniforms);
}

/**
 * Delegate transform to renderer
 * Translate target to position from tilemap
 */
void Target::set_transform(const Transformation& t) {
  Transformation transformation = { glm::translate(glm::mat4(1.0f), m_position), t.view, t.projection };
  renderer.set_transform(transformation);

  // calculate bounding box from positions in local coords in vbo
  // then update bounding box in world coords from model matrix (avoids incremental translation)
  bounding_box = BoundingBox(renderer.vbo.positions);
  bounding_box.transform(transformation.model);
}

/* Free renderer (vao/vbo buffers) & shader program */
void Target::free() {
  renderer.free();
  m_program.free();
}
