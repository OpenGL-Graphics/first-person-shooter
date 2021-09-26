#include <glm/gtc/matrix_transform.hpp>

#include "entities/gun.hpp"
#include "geometries/surface.hpp"
#include "shaders/shader_exception.hpp"

Gun::Gun(Assimp::Importer& importer):
  m_program("assets/shaders/texture_mesh.vert", "assets/shaders/texture_surface.frag"),
  m_model("assets/models/sniper/sniper.obj", importer),
  m_renderer(m_program, m_model, {{0, "position", 3, 8, 0}, {0, "texture_coord", 2, 8, 6}})  // render 3d model
{
  // vertex or fragment shaders failed to compile
  if (m_program.has_failed()) {
    throw ShaderException();
  }
}

/* delegate drawing with OpenGL (buffers & shaders) to renderer */
void Gun::draw(const Uniforms& uniforms) {
  m_renderer.draw(uniforms);
}

/* delegate transform to renderer */
void Gun::set_transform(const Transformation& t) {
  // view = I => fixed translation with camera as origin
  m_renderer.set_transform({
    glm::scale(
      glm::rotate(
        glm::rotate(
          glm::translate(glm::mat4(1.0f), glm::vec3(0.8f, -0.7f, -2.0f)),
          glm::radians(25.0f),
          glm::vec3(0.0f, 1.0f, 0.0f)
        ),
        glm::radians(15.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
      ),
      glm::vec3(0.15f)
    ), glm::mat4(1.0f), t.projection
  });
}

/* Free renderer (vao/vbo buffers), and shader program */
void Gun::free() {
  m_renderer.free();
  m_program.free();
}
