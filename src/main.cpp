#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "texture_2d.hpp"
#include "texture_3d.hpp"
#include "window.hpp"
#include "navigation/frustum.hpp"

#include "geometries/cube.hpp"
#include "geometries/surface.hpp"
#include "geometries/plane.hpp"
#include "geometries/sphere.hpp"
#include "geometries/cylinder.hpp"
#include "geometries/gizmo.hpp"
#include "geometries/grid_lines.hpp"

#include "render/renderer.hpp"
#include "render/text_renderer.hpp"
#include "levels/level_renderer.hpp"
#include "render/model_renderer.hpp"

#include "text/glyphs.hpp"
#include "text/font.hpp"
#include "controls/key_handler.hpp"
#include "controls/mouse_handler.hpp"

#include "profiling/time_profiler.hpp"
#include "profiling/memory_profiler.hpp"

#include "levels/tilemap.hpp"
#include "audio/audio.hpp"

#include "globals/score.hpp"
#include "globals/lights.hpp"

#include "terrain/splatmap.hpp"

#include "framebuffer.hpp"
#include "framebuffer_exception.hpp"
#include "shader_exception.hpp"

#include "factories/shaders_factory.hpp"
#include "factories/textures_factory.hpp"

using namespace irrklang;
using namespace geometry;

int main() {
  ////////////////////////////////////////////////
  // Window & camera
  ////////////////////////////////////////////////

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

  // transformation matrices
  float near = 0.001,
        far = 50.0;
  float aspect_ratio = (float) window.width / (float) window.height;
  glm::mat4 projection3d = glm::perspective(glm::radians(camera.fov), aspect_ratio, near, far);
  glm::mat4 projection2d = glm::ortho(0.0f, (float) window.width, 0.0f, (float) window.height);

  // used to calculate frustum planes (to cull hidden objects => higher fps)
  Frustum frustum(near, far, aspect_ratio);

  ////////////////////////////////////////////////
  // Renderers
  ////////////////////////////////////////////////

  // create & install vertex & fragment shaders on GPU
  ShadersFactory shaders_factory;
  if (shaders_factory.has_failed()) {
    window.destroy();
    throw ShaderException();
  }

  // load textures
  TexturesFactory textures_factory;

  // empty texture to fill when drawing to framebuffer
  Image image_framebuffer(window.width, window.height, GL_RGB, NULL);
  Texture2D texture_framebuffer(image_framebuffer);

  // framebuffers with empty texture attached to it (to render scene to texture)
  Framebuffer framebuffer;
  framebuffer.attach_texture(texture_framebuffer);

  TimeProfiler time_profiler;
  time_profiler.start();

  // renderer (encapsulates VAO & VBO) for each shape to render
  // 08-01-23: ~ total of 40K vertexes coords (float/uint) for geometries => peanuts (not the place to optimize)
  Renderer cubes(shaders_factory["basic"], Cube(), Attributes::get({"position"}, 8));
  Renderer skybox(shaders_factory["skybox"], Cube(true), Attributes::get({"position"}, 8));
  Renderer surface(shaders_factory["texture_surface"], Surface(), Attributes::get({"position", "normal", "texture_coord"}, 7, true));
  Renderer plane(shaders_factory["plane"], Plane(50, 50), Attributes::get({"position", "normal", "texture_coord"}));
  Renderer spheres(shaders_factory["phong"], Sphere(32, 32), Attributes::get({"position", "normal"}));
  Renderer cylinders(shaders_factory["phong"], Cylinder(32, 0.25f, 3.5f), Attributes::get({"position", "normal", "texture_coord", "tangent"}));
  Renderer gizmo(shaders_factory["basic"], Gizmo(), Attributes::get({"position"}));
  Renderer grid(shaders_factory["basic"], GridLines(), Attributes::get({"position"}));

  time_profiler.stop("* Shaders & buffers");

  // renderers names in plural use instancing to render multiple times
  const unsigned int N_LIGHTS = lights.size(),
                     N_CYLINDERS = 2,
                     N_SPHERES = 3;

  // terrain from triangle strips & textured with image splatmap
  Splatmap terrain(shaders_factory["light_terrain"]);

  // accord. to doc: better to reuse importer, & destroys scene (3d model) once out of scope
  Assimp::Importer importer;

  // load font & assign its bitmap glyphs to textures
  Font font("assets/fonts/Vera.ttf");
  TextRenderer surface_glyph(shaders_factory["text"], {{0, "position", 2, 7, 0}, {2, "texture_coord", 2, 7, 5}}, font);

  // load 3d model from .obj file & its renderer
  time_profiler.start();
  assimp_utils::Model model3d_gun("assets/models/sniper/sniper.obj", importer),
                    model3d_suzanne("assets/models/suzanne/suzanne.obj", importer);

  ModelRenderer gun(shaders_factory["texture"], model3d_gun, Attributes::get({"position", "normal", "texture_coord", "tangent"}));
  ModelRenderer suzanne(shaders_factory["texture"], model3d_suzanne, Attributes::get({"position", "normal", "texture_coord", "tangent"}));
  time_profiler.stop("* Loading gun & suzanne 3D models");

  // load tilemap by parsing text file
  time_profiler.start();
  LevelRenderer level(importer, shaders_factory, textures_factory);
  time_profiler.stop("* Loading tilemap, tree & enemy 3D models");
  camera.boundaries = level.positions_walls;

  ////////////////////////////////////////////////
  // Uniforms for cylinders
  ////////////////////////////////////////////////

  std::vector<glm::mat4> models_cylinder = {
    glm::translate(glm::mat4(1.0f), glm::vec3(12, 0, 8)),
    glm::translate(glm::mat4(1.0f), glm::vec3(20, 0, 8))
  };
  std::vector<glm::mat4> normals_mats_cylinders = {
    glm::inverseTranspose(models_cylinder[0]),
    glm::inverseTranspose(models_cylinder[1]),
  };

  ////////////////////////////////////////////////
  // Uniforms for gun
  ////////////////////////////////////////////////

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

  // normal vec to world space (when non-uniform scaling): https://learnopengl.com/Lighting/Basic-Lighting
  glm::mat4 normal_mat_gun = glm::inverseTranspose(model_gun);

  ////////////////////////////////////////////////
  // Uniforms for suzanne
  ////////////////////////////////////////////////

  glm::mat4 model_suzanne = glm::translate(glm::mat4(1.0f), glm::vec3(8.0f, 2.0f, 2.0f));
  glm::mat4 normal_mat_suzanne = glm::inverseTranspose(model_suzanne);

  ////////////////////////////////////////////////
  // Uniforms for health bar
  ////////////////////////////////////////////////

  // scaling then translation with origin at lower left corner
  Texture2D texture_surface_hud = textures_factory.get<Texture2D>("health");
  glm::mat4 model_hud_health(glm::scale(
    glm::translate(
      glm::mat4(1.0f),
      glm::vec3(window.width - texture_surface_hud.width, window.height - texture_surface_hud.height, 0.0f)
    ),
    glm::vec3(texture_surface_hud.width, texture_surface_hud.height, 1.0f)
  ));

  ////////////////////////////////////////////////
  // Uniforms for crosshair
  ////////////////////////////////////////////////

  // crosshair sticked at center of screen
  Texture2D texture_surface_crosshair = textures_factory.get<Texture2D>("crosshair");
  glm::mat4 model_crosshair(glm::scale(
    glm::translate(glm::mat4(1.0f), glm::vec3(
      window.width / 2.0f - texture_surface_crosshair.width / 2.0f,
      window.height / 2.0f - texture_surface_crosshair.height / 2.0f,
      0.0f
    )),
    glm::vec3(texture_surface_crosshair.width, texture_surface_crosshair.height, 1.0f)
  ));

  ////////////////////////////////////////////////
  // Uniforms for light cubes
  ////////////////////////////////////////////////

  // uses instancing to draw light cubes
  // draw 3 light cubes (scaling then translation: transf. matrix = (I * T) * S)
  // https://stackoverflow.com/a/38425832/2228912
  std::vector<glm::mat4> models_lights(N_LIGHTS);
  std::vector<glm::vec3> colors_lights(N_LIGHTS);

  for (size_t i_light = 0; i_light < N_LIGHTS; ++i_light) {
    Light light = lights[i_light];
    models_lights[i_light] = glm::mat4(glm::scale(
      glm::translate(glm::mat4(1.0f), light.position),
      glm::vec3(0.2f)
    ));
    colors_lights[i_light] = light.color;
  }

  // callback for processing mouse click (after init static members)
  MouseHandler::init(&window, &camera, &audio);
  window.attach_mouse_listeners(MouseHandler::on_mouse_move, MouseHandler::on_mouse_click, MouseHandler::on_mouse_scroll);

  // handler for keyboard inputs
  KeyHandler key_handler(window, camera);

  // enable depth test & blending & stencil test (for outlines)
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glEnable(GL_STENCIL_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glm::vec4 background(0.0f, 0.0f, 0.0f, 1.0f);
  glClearColor(background.r, background.g, background.b, background.a);

  // backface (where winding order is CW) not rendered (boost fps)
  glEnable(GL_CULL_FACE);

  // take this line as a ref. to calculate initial fps (not `glfwInit()`)
  window.init_timer();

  ////////////////////////////////////////////////
  // Game loop
  ////////////////////////////////////////////////

  while (!window.is_closed()) {
    // update transformation matrices (camera fov changes on zoom)
    glm::mat4 view = camera.get_view();
    projection3d = glm::perspective(glm::radians(camera.fov), (float) window.width / (float) window.height, 0.5f, 32.0f);

    ///
    // check if origin inside frustum
    frustum.calculate_planes(camera);
    if (frustum.is_inside(glm::vec3()))
      std::cout << "Inside frustum" << '\n';
    else
      std::cout << "Outside frustum" << '\n';
    ///

    {
      // clear framebuffer's attached color buffer in every frame
      framebuffer.bind();
      framebuffer.clear({ 1.0f, 1.0f, 1.0f, 1.0f });

      // draw red cube to texture attached to framebuffer
      cubes.set_transform({
        {
          glm::translate(
            glm::mat4(1.0f),
            glm::vec3(window.width / 2, window.height  / 2, 1.0f)
          )
        },
        view,
        projection2d
      });
      cubes.draw({ {"colors[0]", glm::vec3(1.0f, 0.0f, 0.0f) } });
      framebuffer.unbind();
    }

    // clear color & depth buffers before rendering every frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw skybox
    // https://learnopengl.com/Advanced-OpenGL/Cubemaps
    // disable depth testing so skybox is always at background
    // mandatory bcos of below otherwise cube will hide everything else (coz it closest to camera)
    glDepthMask(GL_FALSE);

    // no translation of skybox when camera moves
    // camera initially at origin always inside skybox unit cube => skybox looks larger
    glm::mat4 view_without_translation = glm::mat4(glm::mat3(view));
    skybox.set_transform({
      { glm::scale(glm::mat4(1.0f), glm::vec3(2)) },
      view_without_translation, projection3d
    });
    skybox.draw({ {"texture3d", textures_factory.get<Texture3D>("skybox") } });
    glDepthMask(GL_TRUE);

    // cube with outline using two-passes rendering & stencil buffer
    glm::mat4 model_cube_outline(glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 5.0f)));
    cubes.set_transform({ {model_cube_outline}, view, projection3d });
    cubes.draw_with_outlines({ {"colors[0]", glm::vec3(0.0f, 0.0f, 1.0f)} });

    // draw textured terrain using triangle strips
    terrain.set_transform({
      { glm::translate(glm::mat4(1.0f), glm::vec3(0, -2.5f, -14)) },
      view, projection3d
    });
    terrain.draw();

    // draw level tiles surfaces on right view
    level.set_transform({ {glm::mat4(1.0f)}, view, projection3d });
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
      {
        glm::scale(
          glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 4.0f)),
          glm::vec3(2.0f, 2.0f * aspect_ratio, 1.0f)
        ),
      },
      view, projection3d
    });
    surface.draw({ {"texture2d", texture_framebuffer } });
    ///

    // draw animated & textured wave from plane using triangle strips
    plane.set_transform({
      { glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 5.0f)) },
      view, projection3d
    });

    plane.draw_plane({
      {"texture2d", textures_factory.get<Texture2D>("wave")},
      {"light.position", lights[0].position},
      {"light.ambiant", 0.2f * lights[0].color},
      {"light.diffuse", 0.5f * lights[0].color},
      {"light.specular", lights[0].color},
      {"time", static_cast<float>(glfwGetTime())},
    });

    // TODO: rotate in vertex shader & move inverseTranspose outside game loop
    // shaded sphere rotating around light
    // https://stackoverflow.com/a/53765106/2228912
    glm::vec3 positions_sphere[N_SPHERES] = {
      glm::vec3( 7.0f, 1.5f, 6.0f),
      glm::vec3(16.0f, 1.5f, 6.0f),
      glm::vec3(26.5f, 1.5f, 6.0f),
    };

    std::vector<glm::mat4> models_spheres(N_SPHERES), normals_mats_spheres(N_SPHERES);
    std::vector<glm::vec3> lights_positions(N_SPHERES), lights_ambiant(N_SPHERES), lights_diffuse(N_SPHERES), lights_specular(N_SPHERES);

    for (size_t i_sphere = 0; i_sphere < N_SPHERES; ++i_sphere) {
      glm::vec3 pivot = lights[i_sphere].position;
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
          positions_sphere[i_sphere] // initial position (also makes radius smaller)
        ),
        glm::vec3(0.5f)
      ));
      models_spheres[i_sphere] = model_sphere;

      // normal vec to world space (when non-uniform scaling): https://learnopengl.com/Lighting/Basic-Lighting
      glm::mat4 normal_mat = glm::inverseTranspose(model_sphere);
      normals_mats_spheres[i_sphere] = normal_mat;

      lights_positions[i_sphere] = lights[i_sphere].position;
      lights_ambiant[i_sphere] = lights[i_sphere].ambiant;
      lights_diffuse[i_sphere] = lights[i_sphere].diffuse;
      lights_specular[i_sphere] = lights[i_sphere].specular;

    } // SPHERES UNIFORMS ARRAYS

    Transformation transform_sphere({ models_spheres, view, projection3d });
    spheres.set_transform(transform_sphere);
    spheres.set_uniform_arr("normals_mats", normals_mats_spheres);
    spheres.set_uniform_arr("lights.position", lights_positions);
    spheres.set_uniform_arr("lights.ambiant", lights_ambiant);
    spheres.set_uniform_arr("lights.diffuse", lights_diffuse);
    spheres.set_uniform_arr("lights.specular", lights_specular);

    spheres.draw({
      {"material.ambiant", glm::vec3(1.0f, 0.5f, 0.31f)},
      {"material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f)},
      {"material.specular", glm::vec3(0.5f, 0.5f, 0.5f)},
      // {"material.shininess", 32.0f},
      {"material.shininess", 4.0f}, // bigger specular reflection

      {"position_camera", camera.position},
    });

    // light cubes
    Transformation transform_cube(models_lights, view, projection3d);
    cubes.set_transform(transform_cube);
    cubes.set_uniform_arr("colors", colors_lights);
    cubes.draw({});

    // uses instancing to draw 2 cylinders pillars (affected by same light source)
    Transformation transform_cylinder(models_cylinder, view, projection3d);
    cylinders.set_transform(transform_cylinder);
    cylinders.set_uniform_arr<glm::mat4>("normals_mats", normals_mats_cylinders);
    cylinders.set_uniform_arr<glm::vec3>("lights.position", { lights[1].position, lights[1].position });
    cylinders.set_uniform_arr<glm::vec3>("lights.ambiant", { lights[1].ambiant, lights[1].ambiant });
    cylinders.set_uniform_arr<glm::vec3>("lights.diffuse", { lights[1].diffuse, lights[1].diffuse });
    cylinders.set_uniform_arr<glm::vec3>("lights.specular", { lights[1].specular, lights[1].specular });

    cylinders.draw({
      {"material.ambiant", glm::vec3(1.0f, 0.5f, 0.31f)},
      {"material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f)},
      {"material.specular", glm::vec3(0.5f, 0.5f, 0.5f)},
      {"material.shininess", 4.0f}, // bigger specular reflection

      {"position_camera", camera.position},
    });

    // draw xyz gizmo at origin using GL_LINES
    gizmo.set_transform({ { glm::mat4(1.0) }, view, projection3d });
    gizmo.draw_lines({ {"colors[0]", glm::vec3(1.0f, 0.0f, 0.0f)} }, 2, 0);
    gizmo.draw_lines({ {"colors[0]", glm::vec3(0.0f, 1.0f, 0.0f)} }, 2, 2);
    gizmo.draw_lines({ {"colors[0]", glm::vec3(0.0f, 0.0f, 1.0f)} }, 2, 4);

    // draw horizontal 2d grid using GL_LINES
    grid.set_transform({ { glm::mat4(1.0) }, view, projection3d });
    grid.draw_lines({ {"colors[0]", glm::vec3(1.0f, 1.0f, 1.0f)} });

    // gun sticked to lower-right corner
    gun.set_transform({ {model_gun}, glm::mat4(1.0f), projection3d });
    gun.set_uniform_arr("normals_mats", { normal_mat_gun });
    gun.draw({
      {"position_camera", camera.position},
      {"positions_lights[0]", lights[0].position},
      {"positions_lights[1]", lights[1].position},
      {"positions_lights[2]", lights[2].position},
    });

    // render 3d model for suzanne with normal mapping
    suzanne.set_transform({ {model_suzanne}, view, projection3d });
    suzanne.set_uniform_arr("normals_mats", { normal_mat_suzanne });
    suzanne.draw({
      {"position_camera", camera.position},
      {"positions_lights[0]", lights[0].position},
      {"positions_lights[1]", lights[1].position},
      {"positions_lights[2]", lights[2].position},
    });

    // draw 2d health bar HUD surface
    surface.set_transform({ {model_hud_health}, glm::mat4(1.0f), projection2d });
    surface.draw({ {"texture2d", texture_surface_hud} });

    // draw crosshair gun target surface at center of screen
    surface.set_transform({ {model_crosshair}, glm::mat4(1.0f), projection2d });
    surface.draw({ {"texture2d", texture_surface_crosshair} });

    // draw 2d text surface (origin: left baseline)
    surface_glyph.set_transform({
      { glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 0.0f)) },
      glm::mat4(1.0f), projection2d
    });
    surface_glyph.draw_text("Score: " + std::to_string(score));

    // process events & show rendered buffer
    window.process_events();
    window.render();

    // keyboard input (move camera, quit application)
    key_handler.on_keypress();

    // continuous jumping/falling after press on <spacebar>
    camera.update();

    // calculate fps
    window.show_fps();

    // MemoryProfiler::profile("Bottom of game loop");
  }

  // destroy textures
  texture_framebuffer.free();

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
  skybox.free();
  surface.free();
  surface_glyph.free();
  plane.free();
  gizmo.free();
  grid.free();

  cubes.free();
  cylinders.free();
  spheres.free();

  // free 3d entities
  gun.free();
  suzanne.free();

  // free shaders programs & textures
  shaders_factory.free();
  textures_factory.free();

  // destroy sound engine & window & terminate glfw
  audio.free();
  window.destroy();

  return 0;
}
