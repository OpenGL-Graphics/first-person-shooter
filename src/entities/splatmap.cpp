#include <glm/gtc/matrix_transform.hpp>

#include "entities/splatmap.hpp"
#include "shader_exception.hpp"
#include "geometries/terrain.hpp"

Splatmap::Splatmap():
  // terrain textures (used by same shader) need to be attached to different texture units
  m_texture_terrain_water(Image("assets/images/terrain/water.jpg"), GL_TEXTURE0),
  m_texture_terrain_grass(Image("assets/images/terrain/grass.jpg"), GL_TEXTURE1),
  m_texture_terrain_rock(Image("assets/images/terrain/rock.jpg"), GL_TEXTURE2),
  m_texture_terrain_splatmap(Image("assets/images/terrain/splatmap.png"), GL_TEXTURE3),

  m_program("assets/shaders/light_terrain.vert", "assets/shaders/light_terrain.frag"),
  m_vbo(Terrain(Image("assets/images/terrain/heightmap.png"))),
  m_renderer(m_program, m_vbo, {{0, "position", 3, 8, 0}, {0, "normal", 3, 8, 3}, {0, "texture_coord", 2, 8, 6}})
{
  // vertex or fragment shaders failed to compile
  if (m_program.has_failed()) {
    throw ShaderException();
  }
}

/* delegate drawing with OpenGL (buffers & shaders) to renderer */
void Splatmap::draw(const Uniforms& uniforms) {
  // draw terrain using triangle strips
  glm::vec3 color_light(1.0f, 1.0f, 1.0f);
  glm::vec3 position_light(10.0f, 6.0f, 6.0f);

  m_renderer.draw(
    {
      {"texture2d_water", m_texture_terrain_water},
      {"texture2d_grass", m_texture_terrain_grass},
      {"texture2d_rock", m_texture_terrain_rock},
      {"texture2d_splatmap", m_texture_terrain_splatmap},
      {"light.position", position_light},
      {"light.ambiant", 0.2f * color_light},
      {"light.diffuse", 0.5f * color_light},
      {"light.specular", color_light},
    }, GL_TRIANGLE_STRIP
  );
}

/* delegate transform to renderer */
void Splatmap::set_transform(const Transformation& t) {
  m_renderer.set_transform(t);
}

/* Free textures, renderer (vao/vbo buffers), and shader program */
void Splatmap::free() {
  m_texture_terrain_water.free();
  m_texture_terrain_grass.free();
  m_texture_terrain_rock.free();
  m_texture_terrain_splatmap.free();

  m_program.free();
  m_renderer.free();
}
