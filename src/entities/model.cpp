#include <glm/gtc/matrix_transform.hpp>

#include "entities/model.hpp"
#include "geometries/surface.hpp"
#include "shader_exception.hpp"

/**
 * @param importer Assimp importer (single instance in all program)
 * @param path Path to 3D model
 * @param program Shaders (texture for gun, basic color for tree)
 */
Model::Model(Assimp::Importer& importer, const std::string& path, const Program& program, const std::vector<Attribute>& attributes):
  m_program(program),
  m_model(path, importer),
  m_renderer(m_program, m_model, attributes)
{
  // vertex or fragment shaders failed to compile
  if (m_program.has_failed()) {
    throw ShaderException();
  }
}

/* delegate drawing with OpenGL (buffers & shaders) to renderer */
void Model::draw(const Uniforms& uniforms) {
  m_renderer.draw(uniforms);
}

/* delegate transform to renderer */
void Model::set_transform(const Transformation<1>& t) {
  m_renderer.set_transform(t);
}

/* Free renderer (vao/vbo buffers), and shader program */
void Model::free() {
  m_renderer.free();
  m_program.free();
}
