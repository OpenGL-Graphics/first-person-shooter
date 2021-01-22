#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <shaders/program.hpp>
#include <navigation/camera.hpp>
#include <geometries/cube.hpp>
#include <geometries/surface.hpp>
#include <materials/texture.hpp>
#include <render/renderer.hpp>
#include <vertexes/vbo.hpp>
// #include <meshes/text.hpp>
#include <gui/dialog.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

// keys & mouse events listeners
static void on_key(GLFWwindow* window);
static void on_mouse_click(GLFWwindow* window, int button, int action, int mods);
static void on_mouse_move(GLFWwindow* window, double xpos, double ypos);
static void on_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset);

// camera (global variable modified inside callbacks)
Camera camera(glm::vec3(0.0f, 5.0f, 10.0f), glm::vec3(0.0f, -0.5f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

// last position of mouse cursor (to calculate offset on movement)
float x_mouse = 0.0f;
float y_mouse = 0.0f;

// background color
const glm::vec4 background(0.0f, 0.0f, 0.0f, 1.0f);

int main() {
  // initialize freetype library
  FT_Library ft;
  if (FT_Init_FreeType(&ft)) {
    std::cout << "Failed to initialize FreeType" << std::endl;
    return 1;
  }

  // load roboto font
  FT_Face face;
  if (FT_New_Face(ft, "assets/fonts/Roboto.ttf", 0, &face)) {
    std::cout << "Failed to load font" << std::endl;
    return 1;
  }

  // set font pixel size
  FT_Set_Pixel_Sizes(face, 0, 48);

  // load font character into 8 bits grayscale bitmap
  FT_Load_Glyph(face, 'X', FT_LOAD_RENDER);
  std::cout << "font width: " << face->glyph->bitmap.width << std::endl;


  // initialize glfw
  if (!glfwInit()) {
    std::cout << "Failed to initialize GLFW" << "\n";
    return 1;
  }

  // get monitor width/height & init mouse position
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);
  const int width_monitor = mode->width;
  const int height_monitor = mode->height;
  x_mouse = width_monitor / 2.0f;
  y_mouse = height_monitor / 2.0f;

  // create window and OpenGL context
  GLFWwindow* window = glfwCreateWindow(width_monitor, height_monitor, "OpenGL test", NULL, NULL);
  if (!window) {
    std::cout << "Failed to create window or OpenGL context" << "\n";
    return 1;
  }

  // initialize glad before calling gl functions
  glfwMakeContextCurrent(window);
  if (!gladLoadGL()) {
    std::cout << "Failed to load Glad (OpenGL)" << "\n";
    return 1;
  } else {
    std::cout << "Opengl version: " << glGetString(GL_VERSION) << "\n";
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
  }

  // callback for processing mouse click
  glfwSetMouseButtonCallback(window, on_mouse_click);

  // create then install vertex & fragment shaders on GPU
  Program pgm_basic("assets/shaders/basic.vert", "assets/shaders/basic.frag");
  Program pgm_color("assets/shaders/color.vert", "assets/shaders/color.frag");
  Program pgm_texture_surface("assets/shaders/texture_surface.vert", "assets/shaders/texture_surface.frag");
  Program pgm_texture_cube("assets/shaders/texture_cube.vert", "assets/shaders/texture_cube.frag");
  Program pgm_light("assets/shaders/light.vert", "assets/shaders/light.frag");
  if (pgm_color.has_failed() || pgm_texture_cube.has_failed() || pgm_texture_surface.has_failed() || pgm_light.has_failed() || pgm_basic.has_failed()) {
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }

  // textures
  std::vector<std::string> paths_textures {
    "assets/images/brick1.jpg",
    "assets/images/brick1.jpg",
    "assets/images/roof.jpg",
    "assets/images/roof.jpg",
    "assets/images/brick2.jpg",
    "assets/images/brick2.jpg",
  };
  Texture3D texture_brick(paths_textures, GL_TEXTURE0);
  Texture2D texture_grass("assets/images/grass.png", GL_TEXTURE1);
  Texture2D texture_hud_health("assets/images/health.png", GL_TEXTURE2);
  Texture2D texture_glass("assets/images/window.png", GL_TEXTURE3);

  // renderer (encapsulates VAO & VBO) for each shape to render
  VBO vbo_cube(Cube{});
  Renderer cube_basic(pgm_basic, vbo_cube, {{0, "position", 3, 12, 0}});
  Renderer cube_color(pgm_color, vbo_cube, {{0, "position", 3, 12, 0}, {0, "color", 3, 12, 3}});
  Renderer cube_texture(pgm_texture_cube, vbo_cube, {{0, "position", 3, 12, 0}, {0, "texture_coord", 3, 12, 6}});
  Renderer cube_light(pgm_light, vbo_cube, {{0, "position", 3, 12, 0}, {0, "normal", 3, 12, 9}});
  Renderer surface(pgm_texture_surface, VBO(Surface()), {{0, "position", 2, 4, 0}, {0, "texture_coord", 2, 4, 2}});

  // initialize dialog with imgui
  Dialog dialog(window, "Dialog title", "Dialog text");

  // enable depth test & blending
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // main loop
  while (!glfwWindowShouldClose(window)) {
    // keyboard input (move camera, quit application)
    on_key(window);

    // before render, clear color buffer & depth buffer
    glClearColor(background.r, background.g, background.b, background.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // transformation matrices
    glm::mat4 view = camera.get_view();
    glm::mat4 projection3d = glm::perspective(glm::radians(camera.get_fov()), (float) width_monitor / (float) height_monitor, 1.0f, 50.f); 
    glm::mat4 projection2d = glm::ortho(0.0f, (float) width_monitor, 0.0f, (float) height_monitor);

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
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f))},
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

    // draw 2d hud surface (scaling then translation to lower left corner)
    glm::mat4 model_hud_health(glm::scale(
      glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
      glm::vec3(texture_hud_health.get_width(), texture_hud_health.get_height(), 1.0f)
    ));
    surface.draw({
      {"model", model_hud_health},
      {"view", glm::mat4(1.0f)},
      {"projection", projection2d},
      {"texture2d", texture_hud_health},
    });

    // draw color cube
    cube_color.draw({
      {"model", glm::mat4(1.0f)},
      {"view", view},
      {"projection", projection3d},
    });

    // draw 2d grass surface (non-centered)
    surface.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 0.0f))},
      {"view",  view},
      {"projection", projection3d},
      {"texture2d", texture_grass},
    });

    // draw half-transparent objects last
    surface.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.0f, 2.0f))},
      {"view",  view},
      {"projection", projection3d},
      {"texture2d", texture_glass},
    });

    // render imgui dialog
    dialog.render();

    // process events & show rendered buffer
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  // destroy imgui
  dialog.free();

  // destroy renderers of each shape (frees vao & vbo)
  cube_basic.free();
  cube_color.free();
  cube_texture.free();
  cube_light.free();
  surface.free();

  // destroy textures
  texture_brick.free();
  texture_grass.free();
  texture_glass.free();
  texture_hud_health.free();

  // destroy shaders programs
  pgm_basic.free();
  pgm_color.free();
  pgm_texture_cube.free();
  pgm_texture_surface.free();
  pgm_light.free();

  // destroy window & terminate glfw
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

static void on_key(GLFWwindow* window) {
  // close window on escape key press
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  // move camera
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.move(Direction::FORWARD);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.move(Direction::BACKWARD);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.move(Direction::LEFT);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.move(Direction::RIGHT);
}

static void on_mouse_click(GLFWwindow* window, int button, int action, int mods) {
  // switch mouse mode & listen for mouse movement/scroll
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      glfwSetCursorPosCallback(window, on_mouse_move);
      glfwSetScrollCallback(window, on_mouse_scroll);
    } else {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      glfwSetCursorPosCallback(window, NULL);
      glfwSetScrollCallback(window, NULL);
    }
  }
}

static void on_mouse_move(GLFWwindow* window, double xpos, double ypos) {
  // see: https://www.reddit.com/r/opengl/comments/831vpb/
  // calculate offset in mouse cursor position
  float x_offset = xpos - x_mouse; 
  float y_offset = ypos - y_mouse; 
  x_mouse = xpos;
  y_mouse = ypos;
  
  // horizontal/vertical rotation around y-axis/x-axis accord. to offset
  camera.rotate(x_offset, y_offset);
}

static void on_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset) {
  // zoom in/out using mouse wheel
  if (yoffset == 1) {
    camera.zoom(Zoom::IN);
  } else if (yoffset == -1) {
    camera.zoom(Zoom::OUT);
  }
}
