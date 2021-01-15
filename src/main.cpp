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
#include <meshes/rectangle.hpp>
#include <materials/texture.hpp>
// #include <meshes/text.hpp>
#include <gui/dialog.hpp>

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
  int index_texture3d = 0;
  int index_texture2d = 1;
  Texture texture3d(paths_textures, index_texture3d);
  Texture texture2d("assets/images/grass.png", index_texture2d);

  // meshes
  CubeColor cube_color(pgm_color);
  CubeTexture cube_texture(pgm_texture_cube, texture3d);
  Cube light(pgm_basic);
  CubeLight cube_light(pgm_light);
  Pyramid pyramid(pgm_light);
  Circle circle(pgm_basic, 36);
  Cylinder cylinder(pgm_basic, 36);
  Sphere sphere(pgm_basic, 12, 12);
  Rectangle rectangle(pgm_texture_surface, texture2d);

  // model & projection matrices for meshes
  glm::mat4 model_cube_color(glm::mat4(1.0f));
  glm::mat4 model_cube_texture;
  glm::mat4 model_cube_light(glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f)));
  glm::mat4 model_pyramid(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 2.0f)));
  glm::mat4 model_circle(glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 3.0f)));
  glm::mat4 model_cylinder(glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 3.0f)));
  glm::mat4 model_sphere(glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 3.0f)));
  glm::mat4 model_text(glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 0.0f)));
  glm::mat4 model_rectangle(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

  // for light: scaling then translation T * S (glm uses column-major order, i.e. transpose)
  glm::vec3 position_light(-2.0f, -1.0f, 2.0f);
  glm::mat4 model_light(glm::scale(
    glm::translate(glm::mat4(1.0f), position_light),
    glm::vec3(0.2f)
  ));

  // projection matrix
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width_monitor/(float)height_monitor, 1.0f, 50.f); 

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

    // camera matrix changed by user input
    glm::mat4 view = camera.get_view();

    /*
    // draw each character using textures
    pgm_text.use();
    pgm_text.set_mat4("model", model_text);
    pgm_text.set_mat4("view", view);
    pgm_text.set_mat4("projection", projection);
    pgm_text.set_int("texture_in", 0);
    pgm_text.set_int("texture_in", index_tex_text - GL_TEXTURE0);
    text.draw();
    */

    // draw color cube (one program run at a time)
    pgm_color.use();
    pgm_color.set_mat4("model", model_cube_color);
    pgm_color.set_mat4("view", view);
    pgm_color.set_mat4("projection", projection);
    cube_color.draw();

    // draw rectangle
    pgm_texture_surface.use();
    pgm_texture_surface.set_int("texture_in", index_texture2d);
    pgm_texture_surface.set_mat4("model", model_rectangle);
    pgm_texture_surface.set_mat4("view", view);
    pgm_texture_surface.set_mat4("projection", projection);
    rectangle.draw();

    // draw 3x texture cubes
    pgm_texture_cube.use();
    model_cube_texture = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
    pgm_texture_cube.set_mat4("model", model_cube_texture);
    pgm_texture_cube.set_mat4("view", view);
    pgm_texture_cube.set_mat4("projection", projection);
    pgm_texture_cube.set_int("texture_in", index_texture3d);
    cube_texture.draw();
    model_cube_texture = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, -1.0f));
    pgm_texture_cube.set_mat4("model", model_cube_texture);
    cube_texture.draw();
    model_cube_texture = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, -2.0f));
    pgm_texture_cube.set_mat4("model", model_cube_texture);
    cube_texture.draw();

    // cube & light colors
    glm::vec3 color(1.0f, 0.5, 0.3);
    glm::vec3 color_light(1.0f, 1.0f, 1.0f);
    glm::vec3 color_pyramid(1.0f, 0.0f, 1.0f);
    glm::vec3 color_circle(1.0f, 1.0f, 0.0f);
    glm::vec3 color_cylinder(0.0f, 1.0f, 0.0f);
    glm::vec3 color_sphere(0.0f, 0.0f, 1.0f);

    // draw light cube
    pgm_basic.use();
    pgm_basic.set_mat4("model", model_light);
    pgm_basic.set_mat4("view", view);
    pgm_basic.set_mat4("projection", projection);
    pgm_basic.set_vec3("color", color_light);
    light.draw();

    // draw circle
    pgm_basic.set_mat4("model", model_circle);
    pgm_basic.set_vec3("color", color_circle);
    circle.draw(GL_LINE);

    // draw cylinder
    pgm_basic.set_mat4("model", model_cylinder);
    pgm_basic.set_vec3("color", color_cylinder);
    cylinder.draw(GL_LINE);

    // draw sphere
    pgm_basic.set_mat4("model", model_sphere);
    pgm_basic.set_vec3("color", color_sphere);
    sphere.draw(GL_LINE);

    // draw illuminated cube
    pgm_light.use();
    pgm_light.set_mat4("model", model_cube_light);
    pgm_light.set_mat4("view", view);
    pgm_light.set_mat4("projection", projection);
    pgm_light.set_vec3("material.ambiant", glm::vec3(1.0f, 0.5f, 0.31f));
    pgm_light.set_vec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
    pgm_light.set_vec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    pgm_light.set_float("material.shininess", 32.0f);
    pgm_light.set_vec3("light.position", position_light);
    pgm_light.set_vec3("light.ambiant", 0.2f * color_light);
    pgm_light.set_vec3("light.diffuse", 0.5f * color_light);
    pgm_light.set_vec3("light.specular", color_light);
    pgm_light.set_vec3("position_camera", camera.m_position);
    cube_light.draw();

    // draw illuminated pyramid
    pgm_light.set_mat4("model", model_pyramid);
    pyramid.draw();

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
  rectangle.free();

  // destroy textures
  texture2d.free();
  texture3d.free();

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
