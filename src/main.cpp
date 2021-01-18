// set imgui to use glad
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
#include <meshes/cube_color.hpp>
#include <meshes/cube_texture.hpp>
#include <meshes/cube_light.hpp>
#include <meshes/pyramid.hpp>
#include <meshes/circle.hpp>
#include <meshes/cylinder.hpp>
#include <meshes/sphere.hpp>
#include <meshes/surface.hpp>
#include <materials/texture.hpp>
// #include <meshes/text.hpp>
#include <gui/dialog.hpp>

#include <glm/gtx/string_cast.hpp>

// functions headers
static void on_key(GLFWwindow* window);
static void on_mouse_click(GLFWwindow* window, int button, int action, int mods);
static void on_mouse_move(GLFWwindow* window, double xpos, double ypos);

// camera
Camera camera(glm::vec3(0.0f, 5.0f, 5.0f), glm::vec3(0.0f, -0.5f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

// last position of mouse cursor (to calculate offset on movement)
float x_mouse = 0.0f;
float y_mouse = 0.0f;

// background color
const glm::vec4 background(0.0f, 0.0f, 0.0f, 1.0f);

int main() {
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
  Program pgm_text("assets/shaders/text.vert", "assets/shaders/text.frag");
  if (pgm_color.has_failed() || pgm_texture_cube.has_failed() || pgm_texture_surface.has_failed() || pgm_light.has_failed() || pgm_basic.has_failed() || pgm_text.has_failed()) {
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

  // meshes
  CubeColor cube_color(pgm_color);
  CubeTexture cube_texture(pgm_texture_cube);
  Cube light(pgm_basic);
  CubeLight cube_light(pgm_light);
  Pyramid pyramid(pgm_light);
  Circle circle(pgm_basic, 36);
  Cylinder cylinder(pgm_basic, 36);
  Sphere sphere(pgm_basic, 12, 12);
  Surface grass(pgm_texture_surface);
  Surface hud_health(pgm_texture_surface);

  // enable depth testing
  glEnable(GL_DEPTH_TEST);

  // initialize dialog with imgui
  Dialog dialog(window, "Dialog title", "Dialog text");

  // freetype-gl
  // Text text(pgm_text, "Hello", 0, 0);

  // main loop
  while (!glfwWindowShouldClose(window)) {
    // keyboard input (move camera, quit application)
    on_key(window);

    // before render, clear color buffer in blue & depth buffer
    glClearColor(background.r, background.g, background.b, background.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // transformation matrices
    glm::mat4 view = camera.get_view();
    glm::mat4 projection3d = glm::perspective(glm::radians(45.0f), (float) width_monitor / (float) height_monitor, 1.0f, 50.f); 
    glm::mat4 projection2d = glm::ortho(0.0f, (float) width_monitor, 0.0f, (float) height_monitor);

    /*
    // draw each character using textures
    pgm_text.use();
    pgm_text.set_mat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 0.0f)));
    pgm_text.set_mat4("view", view);
    pgm_text.set_mat4("projection", projection3d);
    pgm_text.set_int("texture_in", 0);
    pgm_text.set_int("texture_in", index_tex_text - GL_TEXTURE0);
    text.draw();
    */

    // draw 2d hud surface (scaling then translation to lower left corner)
    glm::mat4 model_hud_health(glm::scale(
      glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
      glm::vec3(texture_hud_health.get_width(), texture_hud_health.get_height(), 1.0f)
    ));
    hud_health.draw({
      {"model", model_hud_health},
      {"view", glm::mat4(1.0f)},
      {"projection", projection2d},
      {"texture2d", texture_hud_health.get_index()},
    });

    // draw color cube (one program run at a time)
    cube_color.draw({
      {"model", glm::mat4(1.0f)},
      {"view", view},
      {"projection", projection3d},
    });

    // draw 2d grass surface
    grass.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f))},
      {"view",  view},
      {"projection", projection3d},
      {"texture2d", texture_grass.get_index()},
    });

    // draw 3x texture cubes
    cube_texture.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f))},
      {"view", view},
      {"projection", projection3d},
      {"texture3d", texture_brick.get_index()},
    });
    cube_texture.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, -1.0f))},
      {"view", view},
      {"projection", projection3d},
      {"texture3d", texture_brick.get_index()},
    });
    cube_texture.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, -2.0f))},
      {"view", view},
      {"projection", projection3d},
      {"texture3d", texture_brick.get_index()},
    });

    // cube & light colors
    glm::vec3 color_light(1.0f, 1.0f, 1.0f);

    // draw circle
    circle.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 3.0f))},
      {"view", view},
      {"projection", projection3d},
      {"color", glm::vec3(1.0f, 1.0f, 0.0f)},
    }, GL_LINE);

    // draw cylinder
    cylinder.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 3.0f))},
      {"view", view},
      {"projection", projection3d},
      {"color", glm::vec3(0.0f, 1.0f, 0.0f)},
    }, GL_LINE);

    // draw sphere
    sphere.draw({
        {"model", glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 3.0f))},
        {"view", view},
        {"projection", projection3d},
        {"color", glm::vec3(0.0f, 0.0f, 1.0f)},
    }, GL_LINE);

    // draw light cube (scaling then translation)
    glm::vec3 position_light(-2.0f, -1.0f, 2.0f);
    glm::mat4 model_light(glm::scale(
      glm::translate(glm::mat4(1.0f), position_light),
      glm::vec3(0.2f)
    ));
    light.draw({
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

    // draw illuminated pyramid
    pyramid.draw({
      {"model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 2.0f))},
      {"view", view},
      {"projection", projection3d},
      {"material.ambiant", glm::vec3(1.0f, 0.5f, 0.31f)},
      {"material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f)},
      {"material.specular",glm::vec3(0.5f, 0.5f, 0.5f)},
      {"material.shininess", 32.0f},
      {"light.position", position_light},
      {"light.ambiant", 0.2f * color_light},
      {"light.diffuse", 0.5f * color_light},
      {"light.specular", color_light},
      {"position_camera", camera.get_position()},
    });

    // render imgui dialog
    dialog.render();

    // process events & show rendered buffer
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  // destroy imgui
  dialog.free();

  // destroy meshes
  cube_color.free();
  cube_texture.free();
  cube_light.free();
  light.free();
  circle.free();
  cylinder.free();
  sphere.free();
  // text.free();
  grass.free();
  hud_health.free();

  // destroy textures
  texture_brick.free();
  texture_grass.free();
  texture_hud_health.free();

  // destroy shaders programs
  pgm_basic.free();
  pgm_color.free();
  pgm_texture_cube.free();
  pgm_texture_surface.free();
  pgm_text.free();
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
  // switch mouse mode & listen for mouse movement
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      glfwSetCursorPosCallback(window, on_mouse_move);
    } else {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      glfwSetCursorPosCallback(window, NULL);
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
