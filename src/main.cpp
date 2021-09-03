#include <iostream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "navigation/camera.hpp"
#include "geometries/cube.hpp"
#include "geometries/surface.hpp"
#include "geometries/terrain.hpp"
#include "render/renderer.hpp"
#include "render/text_renderer.hpp"
#include "render/model_renderer.hpp"
#include "render/level_renderer.hpp"
#include "vertexes/vbo.hpp"
#include "text/glyphs.hpp"
#include "text/font.hpp"
#include "gui/dialog.hpp"
#include "gui/window.hpp"
#include "controls/key_handler.hpp"
#include "controls/mouse_handler.hpp"
#include "models/mesh.hpp"
#include "models/model.hpp"
#include "profiling/profiler.hpp"
#include "levels/tilemap.hpp"

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

  // camera & transformation matrixes
  Camera camera(glm::vec3(0.0f, 2.5f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  // create then install vertex & fragment shaders on GPU
  Program pgm_basic("assets/shaders/basic.vert", "assets/shaders/basic.frag");
  Program pgm_color("assets/shaders/color.vert", "assets/shaders/color.frag");
  Program pgm_texture_surface("assets/shaders/texture_surface.vert", "assets/shaders/texture_surface.frag");
  Program pgm_texture_surface_mix("assets/shaders/texture_surface.vert", "assets/shaders/texture_surface_mix.frag");
  Program pgm_texture_mesh("assets/shaders/texture_mesh.vert", "assets/shaders/texture_surface.frag");
  Program pgm_text("assets/shaders/texture_surface.vert", "assets/shaders/texture_text.frag");
  Program pgm_texture_cube("assets/shaders/texture_cube.vert", "assets/shaders/texture_cube.frag");
  Program pgm_light("assets/shaders/light.vert", "assets/shaders/light.frag");
  Program pgm_light_terrain("assets/shaders/light_terrain.vert", "assets/shaders/light_terrain.frag");
  if (pgm_color.has_failed() || pgm_texture_cube.has_failed() || pgm_texture_surface.has_failed() || pgm_texture_surface_mix.has_failed() ||
      pgm_texture_mesh.has_failed() || pgm_light.has_failed() || pgm_basic.has_failed() || pgm_text.has_failed() ||
      pgm_light_terrain.has_failed()) {
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

  // 2D surface textures
  Texture2D texture_surface_grass(Image("assets/images/surfaces/grass.png"));
  Texture2D texture_surface_hud(Image("assets/images/surfaces/health.png"));
  Texture2D texture_surface_glass(Image("assets/images/surfaces/window.png"));
  Texture2D texture_surface_crosshair(Image("assets/images/surfaces/crosshair.png"));

  // terrain textures (used by same shader) need to be attached to different texture units
  Texture2D texture_terrain_water(Image("assets/images/terrain/water.jpg"), GL_TEXTURE0);
  Texture2D texture_terrain_grass(Image("assets/images/terrain/grass.jpg"), GL_TEXTURE1);
  Texture2D texture_terrain_rock(Image("assets/images/terrain/rock.jpg"), GL_TEXTURE2);
  Texture2D texture_terrain_splatmap(Image("assets/images/terrain/splatmap.png"), GL_TEXTURE3);

  // multiple textures in same shader (by attaching them to texture units GL_TEXTURE0/1)
  Texture2D texture_panda(Image("assets/images/surfaces/panda.jpg"), GL_TEXTURE0);
  Texture2D texture_cat(Image("assets/images/surfaces/cat.jpg"), GL_TEXTURE1);

  // renderer (encapsulates VAO & VBO) for each shape to render
  VBO vbo_cube(Cube{});
  Renderer cube_basic(pgm_basic, vbo_cube, {{0, "position", 3, 12, 0}});
  Renderer cube_color(pgm_color, vbo_cube, {{0, "position", 3, 12, 0}, {0, "color", 3, 12, 3}});
  Renderer cube_texture(pgm_texture_cube, vbo_cube, {{0, "position", 3, 12, 0}, {0, "texture_coord", 3, 12, 6}});
  Renderer cube_light(pgm_light, vbo_cube, {{0, "position", 3, 12, 0}, {0, "normal", 3, 12, 9}});
  Renderer surface(pgm_texture_surface, VBO(Surface()), {{0, "position", 2, 4, 0}, {0, "texture_coord", 2, 4, 2}});
  Renderer surface_mix(pgm_texture_surface_mix, VBO(Surface()), {{0, "position", 2, 4, 0}, {0, "texture_coord", 2, 4, 2}});

  // horizontal terrain from triangle strips
  // Renderer terrain(pgm_color, VBO(Terrain(10, 10)), {{0, "position", 3, 6, 0}, {0, "color", 3, 6, 3}});
  // Renderer terrain(pgm_light_terrain, VBO(Terrain(10, 10)), {{0, "position", 3, 8, 0}, {0, "normal", 3, 8, 3}, {0, "texture_coord", 2, 8, 6}});
  VBO vbo_terrain(Terrain(Image("assets/images/terrain/heightmap.png")));
  Renderer terrain(pgm_light_terrain, vbo_terrain, {{0, "position", 3, 8, 0}, {0, "normal", 3, 8, 3}, {0, "texture_coord", 2, 8, 6}});

  // load tilemap by parsing text file
  Tilemap tilemap("assets/levels/map.txt");
  LevelRenderer level(pgm_texture_surface, tilemap);

  // load font & assign its bitmap glyphs to textures
  VBO vbo_glyph(Surface(), true, GL_DYNAMIC_DRAW);
  Font font("assets/fonts/Vera.ttf");
  TextRenderer surface_glyph(pgm_text, vbo_glyph, {{0, "position", 2, 4, 0}, {0, "texture_coord", 2, 4, 2}}, font);

  // accord. to doc: better to reuse importer, & destroys scene (3d model) once out of scope
  Assimp::Importer importer;

  // load 3d model from .obj file & its renderer
  Profiler profiler;
  profiler.start();
  Model model_gun("assets/models/sniper/sniper.obj", importer);
  Model model_two_cubes("assets/models/two-cubes/two-cubes.obj", importer);
  Model model_cube("assets/models/cube-textured/cube-textured.obj", importer);
  Model model_trapezoid("assets/models/trapezoid/trapezoid.obj", importer);

  // renderers for 3d models
  ModelRenderer renderer_gun(pgm_texture_mesh, model_gun, {{0, "position", 3, 8, 0}, {0, "texture_coord", 2, 8, 6}});
  ModelRenderer renderer_trapezoid(pgm_basic, model_trapezoid, {{0, "position", 3, 8, 0}});
  ModelRenderer renderer_two_cubes(pgm_basic, model_two_cubes, {{0, "position", 3, 8, 0}});
  ModelRenderer renderer_player(pgm_texture_mesh, model_cube, {{0, "position", 3, 8, 0}, {0, "texture_coord", 2, 8, 6}});
  Player player(&renderer_player);
  profiler.stop();
  profiler.print("Loading 3D models");

  // transformation matrices
  glm::mat4 view = camera.get_view();
  glm::mat4 projection3d = window.get_projection3d(camera);
  glm::mat4 projection2d = window.get_projection2d();

  // position 3D models
  renderer_trapezoid.set_transform({ glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 25.0f)), view, projection3d });
  renderer_two_cubes.set_transform({ glm::translate(glm::mat4(1.0f), glm::vec3(-7.0f, 0.0f, 0.0f)), view, projection3d });
  renderer_player.set_transform({ glm::mat4(1.0f), view, projection3d });
  player.calculate_bounding_box();

  // cubes to collide with (cube_texture)
  std::vector<glm::vec3> positions = {
    glm::vec3(2.0f, 0.0f, 0.0f),
    glm::vec3(2.0f, 0.0f, -1.0f),
    glm::vec3(2.0f, 0.0f, -2.0f),
  };
  unsigned int score = 0;

  // initialize dialog with imgui
  // Dialog dialog(window, "Dialog title", "Dialog text");

  // targets to mouse cursor intersection
  Target target_cube_basic(&cube_basic);
  Target target_cube_color(&cube_color);
  Target target_cube_texture(&cube_texture);
  Target target_cube_light(&cube_light);
  std::vector<Target *> targets = {&target_cube_basic, &target_cube_color, &target_cube_texture, &target_cube_light};

  // callback for processing mouse click (after init static members)
  MouseHandler::init(&window, &camera, targets);
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

    // keyboard input (move camera, quit application)
    key_handler.on_keypress();

    // clear color & depth & stencil buffers before rendering every frame
    glClearColor(background.r, background.g, background.b, background.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // update transformation matrices
    view = camera.get_view();
    projection3d = window.get_projection3d(camera);

    // cube with outline using two-passes rendering & stencil buffer
    glm::mat4 model_cube_outline(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 5.0f)));
    cube_basic.set_transform({ model_cube_outline, view, projection3d });
    Uniforms uniforms_cube_outline = { {"color", glm::vec3(0.0f, 0.0f, 1.0f)}, };
    cube_basic.draw_with_outlines(uniforms_cube_outline);

    // draw level tiles surfaces
    Uniforms uniforms_level = {
      {"view", view},
      {"projection", projection3d},
    };
    level.set_transform({ glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, 10.0f)), view, projection3d});
    level.draw(uniforms_level);

    // draw terrain using triangle strips
    glm::vec3 color_light(1.0f, 1.0f, 1.0f);
    glm::vec3 position_light(10.0f, 6.0f, 6.0f);
    terrain.set_transform({ glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, -10.0f)), view, projection3d });
    Uniforms uniform_terrain = {
      {"view", view},
      {"projection", projection3d},
      {"texture2d_water", texture_terrain_water},
      {"texture2d_grass", texture_terrain_grass},
      {"texture2d_rock", texture_terrain_rock},
      {"texture2d_splatmap", texture_terrain_splatmap},
      {"light.position", position_light},
      {"light.ambiant", 0.2f * color_light},
      {"light.diffuse", 0.5f * color_light},
      {"light.specular", color_light},
    };
    terrain.draw(uniform_terrain, GL_TRIANGLE_STRIP);

    // draw light cube (scaling then translation: transf. matrix = (I * T) * S)
    // https://stackoverflow.com/a/38425832/2228912
    glm::mat4 model_light(glm::scale(
      glm::translate(glm::mat4(1.0f), position_light),
      glm::vec3(0.2f)
    ));
    cube_basic.set_transform({ model_light, view, projection3d });
    Uniforms uniforms_cube_basic = {
      {"view", view},
      {"projection", projection3d},
      {"color", color_light},
    };
    target_cube_basic.draw(uniforms_cube_basic);

    // draw illuminated cube
    cube_light.set_transform({ glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 4.0f)), view, projection3d });
    Uniforms uniforms_cube_light = {
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
      {"position_camera", camera.position},
    };
    target_cube_light.draw(uniforms_cube_light);

    // draw colored trapezoid 3d model
    Uniforms uniforms_trapezoid = {
      {"view", view},
      {"projection", projection3d},
    };
    renderer_trapezoid.draw(uniforms_trapezoid);

    // draw color cube
    cube_color.set_transform({ glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.0f, 0.0f)), view, projection3d });
    Uniforms uniform_cube_color = {
      {"view", view},
      {"projection", projection3d},
    };
    target_cube_color.draw(uniform_cube_color);

    // draw colored two-cubes 3d model
    Uniforms uniforms_two_cubes = {
      {"view", view},
      {"projection", projection3d},
    };
    renderer_two_cubes.draw(uniforms_two_cubes);

    // draw 3x texture cubes
    Uniforms uniforms_cube_texture = {
      {"view", view},
      {"projection", projection3d},
      {"texture3d", texture_cube},
    };
    std::vector<BoundingBox> bounding_boxes;
    for (const glm::vec3& position : positions) {
      cube_texture.set_transform({ glm::translate(glm::mat4(1.0f), position), view, projection3d });
      target_cube_texture.draw(uniforms_cube_texture);
      bounding_boxes.push_back(cube_texture.bounding_box);
    }

    // draw textured cube 3d model
    Uniforms uniforms_cube_textured = {
      {"view", view},
      {"projection", projection3d},
    };
    renderer_player.draw(uniforms_cube_textured);


    // remove static textured cubes on collision with moving PC & increment score
    int i_bounding_box;
    if ((i_bounding_box = player.bounding_box.check_collision(bounding_boxes)) != BoundingBox::NO_COLLISION) {
      std::cout << "Collision with " << i_bounding_box << '\n';
      positions.erase(positions.begin() + i_bounding_box);
      ++score;
    }

    // draw surface with two blending textures
    surface_mix.set_transform({ glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, 0.0f)), view, projection3d });
    Uniforms uniform_mix = {
      {"view",  view},
      {"projection", projection3d},
      {"texture2d_1", texture_panda},
      {"texture2d_2", texture_cat},
    };
    surface_mix.draw(uniform_mix);

    // draw 2d grass surface (non-centered)
    surface.set_transform({ glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f)), view, projection3d });
    Uniforms uniform_grass = {
      {"view",  view},
      {"projection", projection3d},
      {"texture2d", texture_surface_grass},
    };
    surface.draw(uniform_grass);

    // draw textured gun model with position fixed rel. to camera
    // view = I => fixed translation with camera as origin
    renderer_gun.set_transform({
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
    Uniforms uniforms_gun = {};
    renderer_gun.draw(uniforms_gun);

    // last to render: transparent surfaces to ensure blending with background
    // draw half-transparent 3d window
    surface.set_transform({ glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.0f, 2.0f)), view, projection3d });
    Uniforms uniform_glass = {
      {"view",  view},
      {"projection", projection3d},
      {"texture2d", texture_surface_glass},
    };
    surface.draw(uniform_glass);

    // draw 2d health bar HUD surface (scaling then translation with origin at lower left corner)
    glm::mat4 model_hud_health(glm::scale(
      glm::translate(
        glm::mat4(1.0f),
        glm::vec3(window.width - texture_surface_hud.get_width(), window.height - texture_surface_hud.get_height(), 0.0f)
      ),
      glm::vec3(texture_surface_hud.get_width(), texture_surface_hud.get_height(), 1.0f)
    ));
    surface.set_transform({ model_hud_health, glm::mat4(1.0f), projection2d });
    Uniforms uniform_hud = { {"texture2d", texture_surface_hud}, };
    surface.draw(uniform_hud);

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
    Uniforms uniform_crosshair = { {"texture2d", texture_surface_crosshair}, };
    surface.draw(uniform_crosshair);

    // draw 2d text surface (origin: left baseline)
    surface_glyph.set_transform({ glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 0.0f)), glm::mat4(1.0f), projection2d });
    Uniforms uniforms_text = {};
    surface_glyph.draw_text(uniforms_text, "Score: " + std::to_string(score));

    // render imgui dialog
    // dialog.render();

    // process events & show rendered buffer
    window.process_events();
    window.render();
  }

  // destroy imgui
  // dialog.free();

  // destroy textures
  texture_cube.free();

  texture_surface_grass.free();
  texture_surface_glass.free();
  texture_surface_hud.free();
  texture_surface_crosshair.free();

  texture_terrain_water.free();
  texture_terrain_grass.free();
  texture_terrain_rock.free();
  texture_terrain_splatmap.free();

  texture_cat.free();
  texture_panda.free();

  Glyphs glyphs(surface_glyph.get_glyphs());
  for (unsigned char c = CHAR_START; c <= CHAR_END; c++) {
    Glyph glyph(glyphs.at(c));
    glyph.texture.free();
  }

  // destroy renderers of each shape (frees vao & vbo)
  level.free();
  terrain.free();
  cube_basic.free();
  cube_color.free();
  cube_texture.free();
  cube_light.free();
  surface.free();
  surface_glyph.free();
  renderer_gun.free();
  renderer_trapezoid.free();
  renderer_two_cubes.free();
  renderer_player.free();

  // destroy shaders programs
  pgm_basic.free();
  pgm_color.free();
  pgm_texture_cube.free();
  pgm_texture_surface.free();
  pgm_texture_surface_mix.free();
  pgm_texture_mesh.free();
  pgm_light.free();
  pgm_light_terrain.free();
  pgm_text.free();

  // destroy window & terminate glfw
  window.destroy();

  return 0;
}
