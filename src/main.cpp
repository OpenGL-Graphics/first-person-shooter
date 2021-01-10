// set imgui to use glad
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
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

// functions headers
static void on_key(GLFWwindow* window);
static void on_mouse_click(GLFWwindow* window, int button, int action, int mods);
static void on_mouse_move(GLFWwindow* window, double xpos, double ypos);

// camera
Camera camera(glm::vec3(0.0f, 3.0f, 5.0f), glm::vec3(0.0f, -0.5f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

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
  Program program_color("assets/shaders/color.vert", "assets/shaders/color.frag");
  Program program_texture("assets/shaders/texture.vert", "assets/shaders/texture.frag");
  Program program_light("assets/shaders/light.vert", "assets/shaders/light.frag");
  Program program_basic("assets/shaders/basic.vert", "assets/shaders/basic.frag");
  if (program_color.has_failed() || program_texture.has_failed() || program_light.has_failed() || program_basic.has_failed()) {
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }

  // color & texture cube
  std::vector<std::string> paths_textures {
    "assets/images/brick1.jpg",
    "assets/images/brick1.jpg",
    "assets/images/roof.jpg",
    "assets/images/roof.jpg",
    "assets/images/brick2.jpg",
    "assets/images/brick2.jpg",
  };
  CubeColor cube_color(program_color);
  CubeTexture cube_texture(program_texture, paths_textures);

  // light source cube & illuminated cube
  Cube light(program_basic);
  CubeLight cube_light(program_light);
  Pyramid pyramid(program_light);

  // model & projection matrices for meshes
  glm::mat4 model_cube_color(glm::mat4(1.0f));
  glm::mat4 model_cube_texture(glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f)));
  glm::mat4 model_cube_light(glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f)));
  glm::mat4 model_pyramid(glm::scale(
    glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 2.0f)),
    glm::vec3(2.0f)
  ));

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

  // setup imgui context & glfw/opengl backends
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 130");

  // main loop
  while (!glfwWindowShouldClose(window)) {
    // keyboard input (move camera, quit application)
    on_key(window);

    // start imgui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // imgui window with widgets
    ImGui::Begin("Hello world");
    ImGui::Text("This is a text");
    ImGui::End();
    ImGui::Render();

    // before render, clear color buffer in blue & depth buffer
    glClearColor(background.r, background.g, background.b, background.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // camera matrix changed by user input
    glm::mat4 view = camera.get_view();

    // draw color cube (one program run at a time)
    program_color.use();
    program_color.set_mat4("model", model_cube_color);
    program_color.set_mat4("view", view);
    program_color.set_mat4("projection", projection);
    cube_color.draw();

    // draw texture cube
    program_texture.use();
    program_texture.set_mat4("model", model_cube_texture);
    program_texture.set_mat4("view", view);
    program_texture.set_mat4("projection", projection);
    cube_texture.draw();

    // cube & light colors
    glm::vec3 color(1.0f, 0.5, 0.3);
    glm::vec3 color_light(1.0f, 1.0f, 1.0f);
    glm::vec3 color_pyramid(1.0f, 0.0f, 1.0f);

    // draw light cube
    program_basic.use();
    program_basic.set_mat4("model", model_light);
    program_basic.set_mat4("view", view);
    program_basic.set_mat4("projection", projection);
    program_basic.set_vec3("color", color_light);
    light.draw();

    // draw illuminated cube
    program_light.use();
    program_light.set_mat4("model", model_cube_light);
    program_light.set_mat4("view", view);
    program_light.set_mat4("projection", projection);
    program_light.set_vec3("material.ambiant", glm::vec3(1.0f, 0.5f, 0.31f));
    program_light.set_vec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
    program_light.set_vec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    program_light.set_float("material.shininess", 32.0f);
    program_light.set_vec3("light.position", position_light);
    program_light.set_vec3("light.ambiant", 0.2f * color_light);
    program_light.set_vec3("light.diffuse", 0.5f * color_light);
    program_light.set_vec3("light.specular", color_light);
    program_light.set_vec3("position_camera", camera.m_position);
    cube_light.draw();

    // draw illuminated pyramid
    program_light.set_mat4("model", model_pyramid);
    pyramid.draw();

    // render imgui window
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // process events & show rendered buffer
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  // destroy imgui
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  // de-allocate GPU buffers & textures
  cube_color.free();
  cube_texture.free();
  cube_light.free();
  light.free();

  // destroy shaders programs
  program_color.free();
  program_texture.free();
  program_light.free();
  program_basic.free();

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
