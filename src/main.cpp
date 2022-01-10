#include <iostream>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "window.hpp"
#include "navigation/camera.hpp"
#include "geometries/cube.hpp"
#include "geometries/surface.hpp"
#include "render/renderer.hpp"
#include "render/text_renderer.hpp"
#include "render/model_renderer.hpp"
#include "render/level_renderer.hpp"
#include "text/glyphs.hpp"
#include "text/font.hpp"
#include "controls/key_handler.hpp"
#include "controls/mouse_handler.hpp"
#include "models/mesh.hpp"
#include "models/model.hpp"
#include "profiling/profiler.hpp"
#include "levels/tilemap.hpp"
#include "audio/audio.hpp"
#include "globals/score.hpp"

#include "entities/splatmap.hpp"
#include "entities/model.hpp"
#include "entities/sprite.hpp"

using namespace irrklang;

int main() {
  // glfw window
  Window window("FPS game");

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

  // initialize irrKlang sound engine
  Audio audio;

  // camera
  Camera camera(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  // Camera camera(glm::vec3(0.0f, 2.0f, -2.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  // create then install vertex & fragment shaders on GPU
  // TODO: Factory to produce singletons `Program`s to avoid duplication in Gun & Player
  Program pgm_basic("assets/shaders/basic.vert", "assets/shaders/basic.frag");
  Program pgm_texture("assets/shaders/texture_mesh.vert", "assets/shaders/texture_surface.frag");
  Program pgm_texture_surface("assets/shaders/texture_surface.vert", "assets/shaders/texture_surface.frag");
  Program pgm_text("assets/shaders/texture_surface.vert", "assets/shaders/texture_text.frag");
  Program pgm_texture_cube("assets/shaders/texture_cube.vert", "assets/shaders/texture_cube.frag");
  Program pgm_light("assets/shaders/light.vert", "assets/shaders/light.frag");
  Program pgm_matcap("assets/shaders/matcap.vert", "assets/shaders/matcap.frag");
  if (pgm_texture_cube.has_failed() || pgm_texture.has_failed() || pgm_texture_surface.has_failed() ||
      pgm_light.has_failed() || pgm_basic.has_failed() || pgm_text.has_failed() || pgm_matcap.has_failed()) {
    window.destroy();
    return 1;
  }

  // 3D cube texture
  std::vector<std::string> paths_images {
    "assets/images/cube/brick1.jpg",
    "assets/images/cube/brick1.jpg",
    "assets/images/cube/roof.jpg",
    "assets/images/cube/roof.jpg",
    "assets/images/cube/brick2.jpg",
    "assets/images/cube/brick2.jpg",
  };
  std::vector<Image> images;
  std::transform(paths_images.begin(), paths_images.end(), std::back_inserter(images), [](const std::string& path) { return Image(path); });
  Texture3D texture_cube(images);

  // 2D textures for HUDS
  Texture2D texture_surface_hud(Image("assets/images/surfaces/health.png"));
  Texture2D texture_surface_crosshair(Image("assets/images/surfaces/crosshair.png"));

  // matcap texture
  // Texture2D texture_matcap(Image("assets/images/matcap/046363_0CC3C3_049B9B_04ACAC-512px.png"));
  Texture2D texture_matcap(Image("assets/images/matcap/326666_66CBC9_C0B8AE_52B3B4-512px.png"));

  // renderer (encapsulates VAO & VBO) for each shape to render
  VBO vbo_cube(Cube{});
  Renderer cube_basic(pgm_basic, vbo_cube, {{0, "position", 3, 12, 0}});
  Renderer cube_texture(pgm_texture_cube, vbo_cube, {{0, "position", 3, 12, 0}, {0, "texture_coord", 3, 12, 6}});
  // Renderer cube_light(pgm_light, vbo_cube, {{0, "position", 3, 12, 0}, {0, "normal", 3, 12, 9}});
  Renderer cube_matcap(pgm_matcap, vbo_cube, {{0, "position", 3, 12, 0}, {0, "normal", 3, 12, 9}});
  Renderer surface(pgm_texture_surface, VBO(Surface()), {{0, "position", 2, 4, 0}, {0, "texture_coord", 2, 4, 2}});

  // terrain from triangle strips & textured with image splatmap
  Splatmap terrain;

  // sprites from texture image & 2d surface geometry
  Sprite glass(Image("assets/images/surfaces/window.png"));

  // accord. to doc: better to reuse importer, & destroys scene (3d model) once out of scope
  Assimp::Importer importer;

  // load tilemap by parsing text file
  Tilemap tilemap("assets/levels/map.txt");
  glm::vec3 position_level = {0.0f, 0.0f, 0.0f};
  LevelRenderer level(pgm_texture_surface, tilemap, position_level, importer);
  camera.boundaries = level.positions_walls;

  // load font & assign its bitmap glyphs to textures
  VBO vbo_glyph(Surface(), true, GL_DYNAMIC_DRAW);
  Font font("assets/fonts/Vera.ttf");
  TextRenderer surface_glyph(pgm_text, vbo_glyph, {{0, "position", 2, 4, 0}, {0, "texture_coord", 2, 4, 2}}, font);

  // load 3d model from .obj file & its renderer
  Profiler profiler;
  profiler.start();
  Model gun(importer, "assets/models/sniper/sniper.obj", pgm_texture);
  Player player(importer);
  player.calculate_bounding_box();
  profiler.stop();
  profiler.print("Loading 3D models");

  // transformation matrices
  glm::mat4 view = camera.get_view();
  glm::mat4 projection3d = glm::perspective(glm::radians(camera.fov), (float) window.width / (float) window.height, 1.0f, 50.f);
  glm::mat4 projection2d = glm::ortho(0.0f, (float) window.width, 0.0f, (float) window.height);

  // cubes to collide with (cube_texture)
  std::vector<glm::vec3> positions = {
    glm::vec3(-1.0f, 0.0f, -2.0f),
    glm::vec3(-1.0f, 0.0f, -3.0f),
    glm::vec3(-1.0f, 0.0f, -4.0f),
  };

  // targets to mouse cursor intersection
  /*
  Target target_cube_basic(pgm_basic);
  Target target_cube_texture(pgm_texture_cube);
  Target target_cube_light(pgm_light);
  std::vector<Target *> targets = {&target_cube_basic, &target_cube_texture, &target_cube_light};
  */

  // callback for processing mouse click (after init static members)
  MouseHandler::init(&window, &camera, &audio);
  window.attach_mouse_listeners(MouseHandler::on_mouse_move, MouseHandler::on_mouse_click, MouseHandler::on_mouse_scroll);
  std::cout << "window.width: " << window.width
            << " window.height: " << window.height
            << '\n';

  // handler for keyboard inputs
  KeyHandler key_handler(window, camera, player);

  // enable depth test & blending & stencil test (for outlines)
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glEnable(GL_STENCIL_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glm::vec4 background(0.0f, 0.0f, 0.0f, 1.0f);

  // main loop
  while (!window.is_closed()) {
    // orient player's movements relative to camera horizontal angle (yaw)
    player.orient(camera);

    // clear color & depth & stencil buffers before rendering every frame
    glClearColor(background.r, background.g, background.b, background.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // update transformation matrices (camera fov changes on zoom)
    view = camera.get_view();
    projection3d = glm::perspective(glm::radians(camera.fov), (float) window.width / (float) window.height, 1.0f, 50.f);

    // cube with outline using two-passes rendering & stencil buffer
    glm::mat4 model_cube_outline(glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 5.0f)));
    cube_basic.set_transform({ model_cube_outline, view, projection3d });
    cube_basic.draw_with_outlines({ {"color", glm::vec3(0.0f, 0.0f, 1.0f)} });

    // draw level tiles surfaces
    level.set_transform({ glm::mat4(1.0f), view, projection3d });
    level.draw();

    // draw textured terrain using triangle strips
    terrain.set_transform({ glm::mat4(1.0f), view, projection3d });
    terrain.draw();

    // draw light cube (scaling then translation: transf. matrix = (I * T) * S)
    // https://stackoverflow.com/a/38425832/2228912
    /*
    glm::mat4 model_light(glm::scale(
      glm::translate(glm::mat4(1.0f), position_light),
      glm::vec3(0.2f)
    ));
    cube_basic.set_transform({ model_light, view, projection3d });
    target_cube_basic.draw({ {"color", color_light} });
    */

    // draw illuminated cube
    /*
    cube_light.set_transform({ glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 4.0f, 4.0f)), view, projection3d });
    cube_light.draw({
      {"material.ambiant", glm::vec3(1.0f, 0.5f, 0.31f)},
      {"material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f)},
      {"material.specular", glm::vec3(0.5f, 0.5f, 0.5f)},
      {"material.shininess", 32.0f},
      {"light.position", position_light},
      {"light.ambiant", 0.2f * color_light},
      {"light.diffuse", 0.5f * color_light},
      {"light.specular", color_light},
      {"position_camera", camera.position},
    });
    */

    // draw shaded cube using matcap
    cube_matcap.set_transform({ glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 1.0f, 4.0f)), view, projection3d });
    cube_matcap.draw({
      {"texture2d", texture_matcap},
    });

    // draw color cube (rotated around x-axis)
    /*
    cube_color.set_transform({
      glm::rotate(
        glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.0f, 0.0f)),
        glm::radians(45.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
      ),
      view, projection3d });
    target_cube_color.draw();
    */

    // draw 3x texture cubes
    /*
    std::vector<BoundingBox> bounding_boxes;
    for (const glm::vec3& position : positions) {
      cube_texture.set_transform({ glm::translate(glm::mat4(1.0f), position), view, projection3d });
      target_cube_texture.draw({ {"texture3d", texture_cube} });
      bounding_boxes.push_back(cube_texture.bounding_box);
    }
    */

    // draw textured cube 3d model
    player.set_transform({ glm::mat4(1.0f), view, projection3d });
    player.draw();


    // remove static textured cubes on collision with moving PC
    /*
    int i_bounding_box;
    if ((i_bounding_box = player.bounding_box.check_collision(bounding_boxes)) != BoundingBox::NO_COLLISION) {
      std::cout << "Collision with " << i_bounding_box << '\n';
      positions.erase(positions.begin() + i_bounding_box);
    }
    */

    // draw textured gun model with position fixed rel. to camera
    // view = I => fixed translation with camera as origin
    // stick gun at bottom of screen
    gun.set_transform({
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
      ), glm::mat4(1.0f), projection3d
    });
    gun.draw();

    // last to render: transparent surfaces to ensure blending with background
    glass.set_transform({ glm::translate(glm::mat4(1.0f), glm::vec3(7.0f, 1.0f, 5.0f)), view, projection3d });
    glass.draw();

    // draw 2d health bar HUD surface (scaling then translation with origin at lower left corner)
    glm::mat4 model_hud_health(glm::scale(
      glm::translate(
        glm::mat4(1.0f),
        glm::vec3(window.width - texture_surface_hud.get_width(), window.height - texture_surface_hud.get_height(), 0.0f)
      ),
      glm::vec3(texture_surface_hud.get_width(), texture_surface_hud.get_height(), 1.0f)
    ));
    surface.set_transform({ model_hud_health, glm::mat4(1.0f), projection2d });
    surface.draw({ {"texture2d", texture_surface_hud} });

    // draw crosshair gun target surface at center of screen
    glm::mat4 model_crosshair(glm::scale(
      glm::translate(glm::mat4(1.0f), glm::vec3(
        window.width / 2.0f - texture_surface_crosshair.get_width() / 2.0f,
        window.height / 2.0f - texture_surface_crosshair.get_height() / 2.0f,
        0.0f
      )),
      glm::vec3(texture_surface_crosshair.get_width(), texture_surface_crosshair.get_height(), 1.0f)
    ));
    surface.set_transform({ model_crosshair, glm::mat4(1.0f), projection2d });
    surface.draw({ {"texture2d", texture_surface_crosshair} });

    // draw 2d text surface (origin: left baseline)
    surface_glyph.set_transform({ glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 0.0f)), glm::mat4(1.0f), projection2d });
    surface_glyph.draw_text("Score: " + std::to_string(score));

    // process events & show rendered buffer
    window.process_events();
    window.render();

    // keyboard input (move camera, quit application)
    key_handler.on_keypress();
  }

  // destroy textures
  texture_cube.free();

  texture_surface_hud.free();
  texture_surface_crosshair.free();
  texture_matcap.free();

  Glyphs glyphs(surface_glyph.get_glyphs());
  for (unsigned char c = CHAR_START; c <= CHAR_END; c++) {
    Glyph glyph(glyphs.at(c));
    glyph.texture.free();
  }

  // destroy renderers of each shape (frees vao & vbo)
  level.free();
  terrain.free();
  cube_basic.free();
  cube_texture.free();
  // cube_light.free();
  cube_matcap.free();
  surface.free();
  surface_glyph.free();

  // free 2d & 3d entities
  glass.free();
  gun.free();

  // destroy shaders programs
  pgm_basic.free();
  pgm_texture_cube.free();
  pgm_texture.free();
  pgm_texture_surface.free();
  pgm_light.free();
  pgm_matcap.free();
  pgm_text.free();

  // destroy sound engine & window & terminate glfw
  audio.free();
  window.destroy();

  return 0;
}
