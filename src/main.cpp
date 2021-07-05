#include <iostream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "navigation/camera.hpp"
#include "geometries/cube.hpp"
#include "geometries/surface.hpp"
#include "render/renderer.hpp"
#include "render/text_renderer.hpp"
#include "render/model_renderer.hpp"
#include "vertexes/vbo.hpp"
#include "text/glyphs.hpp"
#include "text/font.hpp"
#include "gui/dialog.hpp"
#include "gui/window.hpp"
#include "models/mesh.hpp"
#include "models/model.hpp"
#include "profiling/profiler.hpp"

// background color
const glm::vec4 background(0.0f, 0.0f, 0.0f, 1.0f);

int main() {
  // initialize glfw
  if (!glfwInit()) {
    std::cout << "Failed to initialize GLFW" << "\n";
    return 1;
  }

  // create window and OpenGL context
  Camera camera(glm::vec3(0.0f, 5.0f, 10.0f), glm::vec3(0.0f, -0.5f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  Window window(&camera);
  if (window.is_null()) {
    std::cout << "Failed to create window or OpenGL context" << "\n";
    return 1;
  }

  // initialize glad before calling gl functions
  window.make_context();
  if (!gladLoadGL()) {
    std::cout << "Failed to load Glad (OpenGL)" << "\n";
    return 1;
  } else {
    std::cout << "Opengl version: " << glGetString(GL_VERSION) << "\n";
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
  }

  // callback for processing mouse click
  window.attach_listeners();

  // create then install vertex & fragment shaders on GPU
  Program pgm_basic("assets/shaders/basic.vert", "assets/shaders/basic.frag");
  Program pgm_color("assets/shaders/color.vert", "assets/shaders/color.frag");
  Program pgm_texture_surface("assets/shaders/texture_surface.vert", "assets/shaders/texture_surface.frag");
  Program pgm_texture_mesh("assets/shaders/texture_mesh.vert", "assets/shaders/texture_surface.frag");
  Program pgm_text("assets/shaders/texture_surface.vert", "assets/shaders/texture_text.frag");
  Program pgm_texture_cube("assets/shaders/texture_cube.vert", "assets/shaders/texture_cube.frag");
  Program pgm_light("assets/shaders/light.vert", "assets/shaders/light.frag");
  if (pgm_color.has_failed() || pgm_texture_cube.has_failed() || pgm_texture_surface.has_failed() || pgm_texture_mesh.has_failed() ||
      pgm_light.has_failed() || pgm_basic.has_failed() || pgm_text.has_failed()) {
    window.destroy();
    return 1;
  }

  // textures
  std::vector<std::string> paths_images {
    "assets/images/brick1.jpg",
    "assets/images/brick1.jpg",
    "assets/images/roof.jpg",
    "assets/images/roof.jpg",
    "assets/images/brick2.jpg",
    "assets/images/brick2.jpg",
  };
  std::vector<Image> images;
  std::transform(paths_images.begin(), paths_images.end(), std::back_inserter(images), [](const std::string& path) { return Image(path); });
  Texture3D texture_brick(images);
  Texture2D texture_grass(Image("assets/images/grass.png"));
  Texture2D texture_hud_health(Image("assets/images/health.png"));
  Texture2D texture_glass(Image("assets/images/window.png"));

  // renderer (encapsulates VAO & VBO) for each shape to render
  VBO vbo_cube(Cube{});
  Renderer cube_basic(pgm_basic, vbo_cube, {{0, "position", 3, 12, 0}});
  Renderer cube_color(pgm_color, vbo_cube, {{0, "position", 3, 12, 0}, {0, "color", 3, 12, 3}});
  Renderer cube_texture(pgm_texture_cube, vbo_cube, {{0, "position", 3, 12, 0}, {0, "texture_coord", 3, 12, 6}});
  Renderer cube_light(pgm_light, vbo_cube, {{0, "position", 3, 12, 0}, {0, "normal", 3, 12, 9}});
  Renderer surface(pgm_texture_surface, VBO(Surface()), {{0, "position", 2, 4, 0}, {0, "texture_coord", 2, 4, 2}});

  // load font & assign its bitmap glyphs to textures
  VBO vbo_glyph(Surface(), true, GL_DYNAMIC_DRAW);
  Font font("assets/fonts/Vera.ttf");
  TextRenderer surface_glyph(pgm_text, vbo_glyph, {{0, "position", 2, 4, 0}, {0, "texture_coord", 2, 4, 2}}, font);

  // accord. to doc: better to reuse importer, & destroys scene (3d model) once out of scope
  Assimp::Importer importer;

  // load 3d model from .obj file & its renderer
  Profiler profiler;
  profiler.start();
  Model two_cubes("assets/models/two-cubes.obj", importer);
  Model backpack("assets/models/backpack.obj", importer);
  ModelRenderer renderer_two_cubes(pgm_basic, two_cubes, {{0, "position", 3, 8, 0}});
  ModelRenderer renderer_backpack(pgm_texture_mesh, backpack, {{0, "position", 3, 8, 0}, {0, "texture_coord", 2, 8, 6}});
  profiler.stop();
  profiler.print("Loading 3D models");

  // initialize dialog with imgui
  // Dialog dialog(window, "Dialog title", "Dialog text");

  // enable depth test & blending
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // main loop
  while (!window.is_closed()) {
    // keyboard input (move camera, quit application)
    window.on_keypress();

    // before render, clear color buffer & depth buffer
    glClearColor(background.r, background.g, background.b, background.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // transformation matrices
    glm::mat4 view = camera.get_view();
    glm::mat4 projection3d = glm::perspective(glm::radians(camera.get_fov()), (float) window.width / (float) window.height, 1.0f, 50.f);
    glm::mat4 projection2d = glm::ortho(0.0f, (float) window.width, 0.0f, (float) window.height);

    // draw 3x texture cubes
    cube_texture.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f))},
      {"view", view},
      {"projection", projection3d},
      {"texture3d", texture_brick},
    });
    cube_texture.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, -1.0f))},
      {"view", view},
      {"projection", projection3d},
      {"texture3d", texture_brick},
    });
    cube_texture.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, -2.0f))},
      {"view", view},
      {"projection", projection3d},
      {"texture3d", texture_brick},
    });

    // cube & light colors
    glm::vec3 color_light(1.0f, 1.0f, 1.0f);

    // draw light cube (scaling then translation)
    glm::vec3 position_light(-2.0f, -1.0f, 2.0f);
    glm::mat4 model_light(glm::scale(
      glm::translate(glm::mat4(1.0f), position_light),
      glm::vec3(0.2f)
    ));
    cube_basic.draw({
      {"model", model_light},
      {"view", view},
      {"projection", projection3d},
      {"color", color_light},
    });

    // draw illuminated cube
    cube_light.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 4.0f))},
      {"view", view},
      {"projection", projection3d},
      {"material.ambiant", glm::vec3(1.0f, 0.5f, 0.31f)},
      {"material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f)},
      {"material.specular", glm::vec3(0.5f, 0.5f, 0.5f)},
      {"material.shininess", 32.0f},
      {"light.position", position_light},
      {"light.ambiant", 0.2f * color_light},
      {"light.diffuse", 0.5f * color_light},
      {"light.specular", color_light},
      {"position_camera", camera.get_position()},
    });

    // draw color cube
    cube_color.draw({
      {"model", glm::mat4(1.0f)},
      {"view", view},
      {"projection", projection3d},
    });

    // draw colored two-cubes 3d model
    Uniforms uniforms_two_cubes = {
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(-7.0f, 0.0f, 0.0f))},
      {"view", view},
      {"projection", projection3d},
    };
    renderer_two_cubes.draw(uniforms_two_cubes);

    // draw textured backpack 3d model
    Uniforms uniforms_backpack = {
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f))},
      {"view", view},
      {"projection", projection3d},
    };
    renderer_backpack.draw(uniforms_backpack);

    // draw 2d grass surface (non-centered)
    surface.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 0.0f))},
      {"view",  view},
      {"projection", projection3d},
      {"texture2d", texture_grass},
    });

    // last to render: transparent surfaces to ensure blending with background
    // draw half-transparent 3d window
    surface.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.0f, 2.0f))},
      {"view",  view},
      {"projection", projection3d},
      {"texture2d", texture_glass},
    });

    // draw 2d health bar HUD surface (scaling then translation to lower left corner)
    glm::mat4 model_hud_health(glm::scale(
      glm::translate(glm::mat4(1.0f), glm::vec3(window.width - texture_hud_health.get_width(), 0.0f, 0.0f)),
      glm::vec3(texture_hud_health.get_width(), texture_hud_health.get_height(), 1.0f)
    ));
    surface.draw({
      {"model", model_hud_health},
      {"view", glm::mat4(1.0f)},
      {"projection", projection2d},
      {"texture2d", texture_hud_health},
    });

    // draw 2d text surface (origin: left baseline)
    Uniforms uniforms_text = {
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 0.0f))},
      {"view", glm::mat4(1.0f)},
      {"projection", projection2d},
    };
    surface_glyph.draw_text(uniforms_text, "Player");

    // render imgui dialog
    // dialog.render();

    // process events & show rendered buffer
    window.process_events();
    window.render();
  }

  // destroy imgui
  // dialog.free();

  // destroy textures
  texture_brick.free();
  texture_grass.free();
  texture_glass.free();
  texture_hud_health.free();

  Glyphs glyphs(surface_glyph.get_glyphs());
  for (unsigned char c = CHAR_START; c <= CHAR_END; c++) {
    Glyph glyph(glyphs.at(c));
    glyph.texture.free();
  }

  // destroy renderers of each shape (frees vao & vbo)
  cube_basic.free();
  cube_color.free();
  cube_texture.free();
  cube_light.free();
  surface.free();
  surface_glyph.free();
  renderer_backpack.free();
  renderer_two_cubes.free();

  // destroy shaders programs
  pgm_basic.free();
  pgm_color.free();
  pgm_texture_cube.free();
  pgm_texture_surface.free();
  pgm_texture_mesh.free();
  pgm_light.free();
  pgm_text.free();

  // destroy window & terminate glfw
  window.destroy();

  return 0;
}
