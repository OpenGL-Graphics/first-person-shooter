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
#include <meshes/cube.hpp>

// camera
Camera camera;
glm::vec3 pos_camera(0.0f, 0.0f, 5.0f);

// last position of mouse cursor (to calculate offset on movement)
float x_mouse = 0.0f;
float y_mouse = 0.0f;

// functions headers
static void on_key(GLFWwindow* window);
static void on_mouse_move(GLFWwindow* window, double xpos, double ypos);

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

  // callback for processing keyboard & mouse inputs
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, on_mouse_move);

  // create then install vertex & fragment shaders on GPU
  Program program_shaders("assets/shaders/cube.vert", "assets/shaders/cube.frag");
  GLuint program = program_shaders.m_id;
  if (program == 0) {
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }
  program_shaders.use();

  // cube with its texture
  std::vector<std::string> names_attributes {"position", "color", "texture_dir"};
  std::vector<std::string> paths_textures {
    "assets/images/brick1.jpg",
    "assets/images/brick1.jpg",
    "assets/images/roof.jpg",
    "assets/images/roof.jpg",
    "assets/images/brick2.jpg",
    "assets/images/brick2.jpg",
  };
  Cube cube(program, names_attributes, paths_textures);

  // projection matrix (3/3): transform to ndc coord
  glm::mat4 projection_mat = glm::perspective(glm::radians(45.0f), (float)width_monitor/(float)height_monitor, 1.0f, 10.f); 
  program_shaders.set_mat4("projection", projection_mat);

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
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // view matrix (2/3): transform to camera coord
    glm::mat4 view_mat = camera.get_view_mat();
    program_shaders.set_mat4("view", view_mat);

    // draw multiple cubes
    for (char i_cube = -2; i_cube <= 2; i_cube += 2) {
      // model matrix (1/3): translate cubes in world coordinates
      glm::mat4 model_mat(1.0f);
      model_mat = glm::translate(model_mat, glm::vec3(i_cube, 0.0f, 0.0f));
      program_shaders.set_mat4("model", model_mat);

      // draw cube
      cube.draw();
    }

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
  cube.free();

  // destroy shaders, window & terminate glfw
  program_shaders.free();
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
