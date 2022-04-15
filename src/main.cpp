#include <iostream>
#include <vector>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "window.hpp"
#include "navigation/camera.hpp"

#include "geometries/cube.hpp"
#include "geometries/surface.hpp"
#include "geometries/plane.hpp"
#include "geometries/sphere.hpp"
#include "geometries/cylinder.hpp"
#include "geometries/gizmo.hpp"
#include "geometries/grid_lines.hpp"

#include "render/renderer.hpp"
#include "render/text_renderer.hpp"
#include "render/level_renderer.hpp"

#include "text/glyphs.hpp"
#include "text/font.hpp"
#include "controls/key_handler.hpp"
#include "controls/mouse_handler.hpp"

#include "profiling/profiler.hpp"
#include "levels/tilemap.hpp"
#include "audio/audio.hpp"

#include "globals/score.hpp"
#include "globals/lights.hpp"

#include "entities/splatmap.hpp"
#include "entities/model.hpp"
#include "entities/sprite.hpp"

#include "framebuffer.hpp"
#include "framebuffer_exception.hpp"
#include "shader_exception.hpp"

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
    window.destroy();
    return 1;
  } else {
    std::cout << "Opengl version: " << glGetString(GL_VERSION) << "\n";
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
  }

  // initialize irrKlang sound engine
  Audio audio;

  // camera
  Camera camera(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  // Camera camera(glm::vec3(13.0f, 2.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  // create then install vertex & fragment shaders on GPU
  // TODO: Factory to produce singletons `Program`s to avoid duplication in Gun & Player
  Program pgm_basic("assets/shaders/basic.vert", "assets/shaders/basic.frag");
  Program pgm_texture("assets/shaders/texture_mesh.vert", "assets/shaders/texture_mesh.frag");
  Program pgm_texture_surface("assets/shaders/texture_surface.vert", "assets/shaders/texture_surface.frag");
  Program pgm_tile("assets/shaders/tile.vert", "assets/shaders/tile.frag");
  Program pgm_text("assets/shaders/texture_surface.vert", "assets/shaders/texture_text.frag");
  Program pgm_texture_cube("assets/shaders/texture_cube.vert", "assets/shaders/texture_cube.frag");
  Program pgm_light("assets/shaders/light.vert", "assets/shaders/light.frag");
  Program pgm_matcap("assets/shaders/matcap.vert", "assets/shaders/matcap.frag");
  Program pgm_plane("assets/shaders/light_plane.vert", "assets/shaders/light_plane.frag");
  if (pgm_texture_cube.has_failed() || pgm_texture.has_failed() || pgm_texture_surface.has_failed() || pgm_tile.has_failed() ||
      pgm_light.has_failed() || pgm_basic.has_failed() || pgm_text.has_failed() || pgm_matcap.has_failed() ||
      pgm_plane.has_failed()) {
    window.destroy();
    throw ShaderException();
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

  // matcap texture (use 2nd texture slot as 1st one used for 3d model's texture, ie. Suzanne)
  // Texture2D texture_matcap(Image("assets/images/matcap/046363_0CC3C3_049B9B_04ACAC-512px.png"), GL_TEXTURE1);
  Texture2D texture_matcap(Image("assets/images/matcap/326666_66CBC9_C0B8AE_52B3B4-512px.png"), GL_TEXTURE1);

  // 2D texture for flat grid plane (shape made as a sin wave in vertex shader)
  Texture2D texture_plane(Image("assets/images/plane/wave.png"));

  // texture for cylinder pillar
  Texture2D texture_cylinder_diffuse(Image("assets/images/level/wall_diffuse.jpg"), GL_TEXTURE0);
  Texture2D texture_cylinder_normal(Image("assets/images/level/wall_normal.jpg"), GL_TEXTURE1);

  // empty texture to fill when drawing to framebuffer
  Image image_framebuffer(window.width, window.height, GL_RGB, NULL);
  Texture2D texture_framebuffer(image_framebuffer);

  // framebuffers with empty texture attached to it (to render scene to texture)
  Framebuffer framebuffer;
  framebuffer.attach_texture(texture_framebuffer);

  if (!framebuffer.is_complete()) {
    window.destroy();
    throw FramebufferException();
  }

  // renderer (encapsulates VAO & VBO) for each shape to render
  VBO vbo_cube(Cube{});
  Renderer cube_basic(pgm_basic, vbo_cube, {{0, "position", 3, 12, 0}});
  Renderer cube_texture(pgm_texture_cube, vbo_cube, {{0, "position", 3, 12, 0}, {1, "texture_coord", 3, 12, 6}});
  // Renderer cube_light(pgm_light, vbo_cube, {{0, "position", 3, 12, 0}, {0, "normal", 3, 12, 9}});
  Renderer cube_matcap(pgm_matcap, vbo_cube, {{0, "position", 3, 12, 0}, {1, "normal", 3, 12, 9}});
  Renderer surface(pgm_texture_surface, VBO(Surface()), {{0, "position", 2, 7, 0}, {1, "normal", 3, 7, 2}, {2, "texture_coord", 2, 7, 5}});
  Renderer plane(pgm_plane, VBO(Plane(50, 50)), {{0, "position", 3, 8, 0}, {1, "normal", 3, 8, 3}, {2, "texture_coord", 2, 8, 6}});
  Renderer sphere(pgm_light, VBO(Sphere(32, 32)), {{0, "position", 3, 6, 0}, {1, "normal", 3, 6, 3}});
  Renderer cylinder(pgm_texture, VBO(Cylinder(32, 0.25f, 3.5f)), {
    {0, "position", 3, 11, 0},
    {1, "normal", 3, 11, 3},
    {2, "texture_coord", 2, 11, 6},
    {3, "tangent", 3, 11, 8},
  });
  Renderer gizmo(pgm_basic, VBO(Gizmo()), { {0, "position", 3, 3, 0} });
  Renderer grid(pgm_basic, VBO(GridLines()), { {0, "position", 3, 3, 0} });

  // terrain from triangle strips & textured with image splatmap
  Splatmap terrain;

  // accord. to doc: better to reuse importer, & destroys scene (3d model) once out of scope
  Assimp::Importer importer;

  // load tilemap by parsing text file
  Tilemap tilemap("assets/levels/map.txt");
  glm::vec3 position_level = {0.0f, 0.0f, 0.0f};
  LevelRenderer level(pgm_tile, tilemap, position_level, importer);
  camera.boundaries = level.positions_walls;

  // load font & assign its bitmap glyphs to textures
  VBO vbo_glyph(Surface(), true, GL_DYNAMIC_DRAW);
  Font font("assets/fonts/Vera.ttf");
  TextRenderer surface_glyph(pgm_text, vbo_glyph, {{0, "position", 2, 7, 0}, {2, "texture_coord", 2, 7, 5}}, font);

  // load 3d model from .obj file & its renderer
  Profiler profiler;
  profiler.start();
  Model gun(importer, "assets/models/sniper/sniper.obj", pgm_texture, {
    {0, "position", 3, 11, 0},
    {1, "normal", 3, 11, 3},
    {2, "texture_coord", 2, 11, 6},
    {3, "tangent", 3, 11, 8},
  });
  Model suzanne(importer, "assets/models/suzanne/suzanne.obj", pgm_texture, {
    {0, "position", 3, 11, 0},
    {1, "normal", 3, 11, 3},
    {2, "texture_coord", 2, 11, 6},
    {3, "tangent", 3, 11, 8},
  });
  Player player(importer);
  player.calculate_bounding_box();
  profiler.stop();
  profiler.print("Loading 3D models");

  // transformation matrices
  glm::mat4 view;
  glm::mat4 projection3d = glm::perspective(glm::radians(camera.fov), (float) window.width / (float) window.height, 1.0e-3f, 50.f);
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

    // update transformation matrices (camera fov changes on zoom)
    view = camera.get_view();
    projection3d = glm::perspective(glm::radians(camera.fov), (float) window.width / (float) window.height, 0.5f, 32.0f);

    {
      // clear framebuffer's attached color buffer in every frame
      framebuffer.bind();
      framebuffer.clear({ 1.0f, 1.0f, 1.0f, 1.0f });

      // draw red cube to texture attached to framebuffer
      cube_basic.set_transform({
        glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 1.0f, 0.0f)),
        view, projection3d });
      cube_basic.draw({ {"color", glm::vec3(1.0f, 0.0f, 0.0f) } });
      framebuffer.unbind();
    }

    // clear color & depth & stencil buffers before rendering every frame
    glClearColor(background.r, background.g, background.b, background.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // cube with outline using two-passes rendering & stencil buffer
    // must be called just after clearing the stencil buffer (before any other drawing)
    glm::mat4 model_cube_outline(glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 5.0f)));
    cube_basic.set_transform({ model_cube_outline, view, projection3d });
    cube_basic.draw_with_outlines({ {"color", glm::vec3(0.0f, 0.0f, 1.0f)} });

    // draw level tiles surfaces on right view
    level.set_transform({ glm::mat4(1.0f), view, projection3d });
    level.draw({
      {"position_camera", camera.position},

      {"positions_lights[0]", lights[0].position},
      {"positions_lights[1]", lights[1].position},
      {"positions_lights[2]", lights[2].position},
    });

    /*
    {
      // aspect ratio prevent view from being stretched
      projection3d = glm::perspective(glm::radians(camera.fov), window.width / (2.0f * window.height), 1.0f, 50.f);
      // projection2d = glm::ortho(0.0f, (float) window.width / 2.0f, 0.0f, (float) window.height);

      // right view
      // where to project rendered scene on window (origin at lower-left corner)
      glViewport(window.width / 2, 0, window.width / 2, window.height);

      // draw level tiles surfaces on right view
      level.set_transform({ glm::mat4(1.0f), view, projection3d });
      level.draw({
        {"position_light", lights[0].position},
      });

      // left view
      // where to project rendered scene on window (origin at lower-left corner)
      glViewport(0, 0, window.width / 2, window.height);

      // draw level tiles surfaces on left view
      level.set_transform({ glm::mat4(1.0f), view, projection3d });
      level.draw({
        {"position_light", lights[0].position},
      });
    }
    */

    ///
    // apply to surface the texture drawn to framebuffer
    // same aspect ratio for surface as texture (to avoid stretching its content)
    float aspect_ratio = (float) window.height / window.width;
    surface.set_transform({
      glm::scale(
        glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 4.0f)),
        glm::vec3(2.0f, 2.0f * aspect_ratio, 1.0f)
      ),
      view, projection3d
    });
    surface.draw({ {"texture2d", texture_framebuffer } });
    ///

    // draw textured terrain using triangle strips
    terrain.set_transform({ glm::mat4(1.0f), view, projection3d });
    terrain.draw();

    // draw animated & textured wave from plane using triangle strips
    plane.set_transform({ glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 5.0f)), view, projection3d });

    plane.draw({
      {"texture2d", texture_plane},
      {"light.position", lights[0].position},
      {"light.ambiant", 0.2f * lights[0].color},
      {"light.diffuse", 0.5f * lights[0].color},
      {"light.specular", lights[0].color},
      {"time", static_cast<float>(glfwGetTime())},
    }, GL_TRIANGLE_STRIP);

    // shaded sphere rotating around light
    // https://stackoverflow.com/a/53765106/2228912
    glm::vec3 positions_sphere[3] = {
      glm::vec3( 7.0f, 1.5f, 6.0f),
      glm::vec3(16.0f, 1.5f, 6.0f),
      glm::vec3(26.5f, 1.5f, 6.0f),
    };

    for (size_t i_light = 0; i_light < 3; ++i_light) {
      glm::vec3 pivot = lights[i_light].position;
      glm::mat4 model_sphere(glm::scale(
        glm::translate(
          glm::translate(
            glm::rotate(
              glm::translate(glm::mat4(1.0f), pivot), // moving pivot to its original pos.
              static_cast<float>(glfwGetTime()),
              glm::vec3(0.0f, 1.0f, 0.0f)
            ),
            -pivot // bringing pivot to origin first
          ),
          positions_sphere[i_light] // initial position (also makes radius smaller)
        ),
        glm::vec3(0.5f)
      ));

      // calculate normal matrix only once (instead of doing it in shader for every vertex)
      // normal vec to world space (when non-uniform scaling): https://learnopengl.com/Lighting/Basic-Lighting
      glm::mat4 normal_mat = glm::inverseTranspose(model_sphere);

      sphere.set_transform({ model_sphere, view, projection3d });
      sphere.draw({
        {"material.ambiant", glm::vec3(1.0f, 0.5f, 0.31f)},
        {"material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f)},
        {"material.specular", glm::vec3(0.5f, 0.5f, 0.5f)},
        // {"material.shininess", 32.0f},
        {"material.shininess", 4.0f}, // bigger specular reflection

        {"light.position", lights[i_light].position},
        {"light.ambiant", 0.2f * lights[i_light].color},
        {"light.diffuse", 0.5f * lights[i_light].color},
        {"light.specular", lights[i_light].color},

        {"position_camera", camera.position},

        {"normal_mat", normal_mat},
      });
    }

    // draw 3 light cubes (scaling then translation: transf. matrix = (I * T) * S)
    // https://stackoverflow.com/a/38425832/2228912
    for (size_t i_light = 0; i_light < 3; ++i_light) {
      glm::mat4 model_light(glm::scale(
        glm::translate(glm::mat4(1.0f), lights[i_light].position),
        glm::vec3(0.2f)
      ));
      cube_basic.set_transform({ model_light, view, projection3d });
      cube_basic.draw({ {"color", lights[i_light].color} });
    }

    // draw a 2 textured cylinders (pillars
    glm::mat4 model_cylinder1 = glm::translate(glm::mat4(1.0f), glm::vec3(12, 0, 8));
    cylinder.set_transform({ model_cylinder1, view, projection3d });
    cylinder.draw({
      {"position_camera", camera.position},
      {"positions_lights[0]", lights[0].position},
      {"positions_lights[1]", lights[1].position},
      {"positions_lights[2]", lights[2].position},
      {"normal_mat", glm::inverseTranspose(model_cylinder1)},
      {"texture_diffuse", texture_cylinder_diffuse},
      {"texture_normal", texture_cylinder_normal},
    });

    glm::mat4 model_cylinder2 = glm::translate(glm::mat4(1.0f), glm::vec3(20, 0, 8));
    cylinder.set_transform({ model_cylinder2, view, projection3d });
    cylinder.draw({
      {"position_camera", camera.position},
      {"positions_lights[0]", lights[0].position},
      {"positions_lights[1]", lights[1].position},
      {"positions_lights[2]", lights[2].position},
      {"normal_mat", glm::inverseTranspose(model_cylinder2)},
      {"texture_diffuse", texture_cylinder_diffuse},
      {"texture_normal", texture_cylinder_normal},
    });

    // draw xyz gizmo at origin using GL_LINES
    glm::mat4 model_gizmo(1.0f);
    gizmo.set_transform({ model_gizmo, view, projection3d });
    gizmo.draw({ {"color", glm::vec3(1.0f, 0.0f, 0.0f)} }, GL_LINES, 2, 0);
    gizmo.draw({ {"color", glm::vec3(0.0f, 1.0f, 0.0f)} }, GL_LINES, 2, 2);
    gizmo.draw({ {"color", glm::vec3(0.0f, 0.0f, 1.0f)} }, GL_LINES, 2, 4);

    // draw horizontal 2d grid using GL_LINES
    glm::mat4 model_grid(1.0f);
    grid.set_transform({ model_grid, view, projection3d });
    grid.draw({ {"color", glm::vec3(1.0f, 1.0f, 1.0f)} }, GL_LINES);


    // draw illuminated cube
    /*
    cube_light.set_transform({ glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 4.0f, 4.0f)), view, projection3d });
    cube_light.draw({
      {"material.ambiant", glm::vec3(1.0f, 0.5f, 0.31f)},
      {"material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f)},
      {"material.specular", glm::vec3(0.5f, 0.5f, 0.5f)},
      {"material.shininess", 32.0f},
      {"light.position", lights[0].position},
      {"light.ambiant", 0.2f * lights[0].color},
      {"light.diffuse", 0.5f * lights[0].color},
      {"light.specular", lights[0].color},
      {"position_camera", camera.position},
    });
    */

    // draw shaded cube using matcap
    cube_matcap.set_transform({ glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 6.0f)), view, projection3d });
    cube_matcap.draw({
      {"texture_matcap", texture_matcap},
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
    glm::mat4 model_gun = glm::scale(
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
    );

    // calculate normal matrix only once (instead of doing it in shader for every vertex)
    // normal vec to world space (when non-uniform scaling): https://learnopengl.com/Lighting/Basic-Lighting
    glm::mat4 normal_mat_gun = glm::inverseTranspose(model_gun);

    gun.set_transform({ model_gun, glm::mat4(1.0f), projection3d });
    gun.draw({
      {"position_camera", camera.position},
      {"positions_lights[0]", lights[0].position},
      {"positions_lights[1]", lights[1].position},
      {"positions_lights[2]", lights[2].position},

      {"normal_mat", normal_mat_gun},
    });

    // render 3d model for suzanne with matcap shader
    glm::mat4 model_suzanne = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 2.0f, 2.0f));
    glm::mat4 normal_mat_suzanne = glm::inverseTranspose(model_suzanne);
    suzanne.set_transform({ model_suzanne, view, projection3d });
    suzanne.draw({
      {"position_camera", camera.position},
      {"positions_lights[0]", lights[0].position},
      {"positions_lights[1]", lights[1].position},
      {"positions_lights[2]", lights[2].position},

      {"normal_mat", normal_mat_suzanne},
    });

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

    // continuous jumping/falling after press on <spacebar>
    camera.update();
  }

  // destroy textures
  texture_cube.free();

  texture_surface_hud.free();
  texture_surface_crosshair.free();
  texture_matcap.free();
  texture_plane.free();
  texture_framebuffer.free();

  texture_cylinder_diffuse.free();
  texture_cylinder_normal.free();

  Glyphs glyphs(surface_glyph.get_glyphs());
  for (unsigned char c = CHAR_START; c <= CHAR_END; c++) {
    Glyph glyph(glyphs.at(c));
    glyph.texture.free();
  }

  // destroy framebuffers
  framebuffer.free();

  // destroy renderers of each shape (frees vao & vbo)
  level.free();
  terrain.free();
  cube_basic.free();
  cube_texture.free();
  // cube_light.free();
  cube_matcap.free();
  surface.free();
  surface_glyph.free();
  plane.free();
  sphere.free();
  cylinder.free();
  gizmo.free();
  grid.free();

  // free 3d entities
  gun.free();
  suzanne.free();

  // destroy shaders programs
  pgm_basic.free();
  pgm_texture_cube.free();
  pgm_texture.free();
  pgm_texture_surface.free();
  pgm_tile.free();
  pgm_light.free();
  pgm_text.free();
  pgm_matcap.free();
  pgm_plane.free();

  // destroy sound engine & window & terminate glfw
  audio.free();
  window.destroy();

  return 0;
}
