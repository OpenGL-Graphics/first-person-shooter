// set imgui to use glad
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

static std::string read_file(const std::string& filename) {
  std::ifstream f(filename.c_str());
  std::stringstream buffer;
  buffer << f.rdbuf();

  return buffer.str();
}

static void on_key(GLFWwindow* window, int key, int scancode, int action, int mods) {
  // close window on escape key press
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

static GLuint create_shaders(const std::string& source_vertex, const std::string& source_fragment) {
  // The Cherno: https://www.youtube.com/watch?v=71BLZwRGUJE
  const char* source_vertex_char = source_vertex.c_str();
  const char* source_fragment_char = source_fragment.c_str();

  // compile shaders
  GLuint shader_vertex= glCreateShader(GL_VERTEX_SHADER);
  GLuint shader_fragment= glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(shader_vertex, 1, &source_vertex_char, NULL);
  glShaderSource(shader_fragment, 1, &source_fragment_char, NULL);
  glCompileShader(shader_vertex);
  glCompileShader(shader_fragment);

  // TODO: group both shaders in same function (refactoring)
  // error handling for vertex shader compilation
  GLint result1;
  glGetShaderiv(shader_vertex, GL_COMPILE_STATUS, &result1);
  if (result1 == GL_FALSE) {
    GLint length1;
    glGetShaderiv(shader_vertex, GL_INFO_LOG_LENGTH, &length1);
    std::vector<GLchar> message(length1);
    glGetShaderInfoLog(shader_vertex, length1, NULL, message.data());
    std::cout << "Vertex shader: " << message.data() << "\n";

    glDeleteShader(shader_vertex);
    return 0;
  }

  // error handling for fragment shader compilation
  GLint result2;
  glGetShaderiv(shader_fragment, GL_COMPILE_STATUS, &result2);
  if (result2 == GL_FALSE) {
    GLint length2;
    glGetShaderiv(shader_fragment, GL_INFO_LOG_LENGTH, &length2);
    std::vector<GLchar> message(length2);
    glGetShaderInfoLog(shader_fragment, length2, NULL, message.data());
    std::cout << "Fragment shader: " << message.data() << "\n";

    glDeleteShader(shader_fragment);
    return 0;
  }

  // attach shaders to program & link it
  GLuint program = glCreateProgram();
  glAttachShader(program, shader_vertex);
  glAttachShader(program, shader_fragment);
  glLinkProgram(program);
  glValidateProgram(program);

  // error handling of program linking
  GLint result_link;
  glGetProgramiv(program,  GL_LINK_STATUS, &result_link);
  if (result_link == GL_FALSE) {
    GLint length_link;
    glGetProgramiv(program,  GL_INFO_LOG_LENGTH, &length_link);
    std::vector<GLchar> message(length_link);
    glGetProgramInfoLog(program, length_link, NULL, message.data());
    std::cout << "program: " << message.data() << "\n";

    glDeleteProgram(program);
    return 0;
  }

  // flag attached shaders objects for deletion
  glDeleteShader(shader_vertex);
  glDeleteShader(shader_fragment);

  return program;
}

int main() {
  // initialize glfw
  if (!glfwInit()) {
    std::cout << "Failed to initialize GLFW" << "\n";
    return 1;
  }

  // create window and OpenGL context
  GLFWwindow* window = glfwCreateWindow(640, 480, "Window title", NULL, NULL);
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

  // callback for processing key press
  glfwSetKeyCallback(window, on_key);

  // GPU buffer (vbo) for vertexes positions
  const float positions[6] = {
    -0.5, -0.5,
     0,    0.5,
     0.5, -0.5
  };
  GLuint index_buffer;
  glGenBuffers(1, &index_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, index_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

  // buffer attributes (position, color...)
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);
  glEnableVertexAttribArray(0);

  // shader source codes (newer GLSL version not supported)
  std::string source_vertex = read_file("assets/shaders/triangle.vert");
  std::string source_fragment = read_file("assets/shaders/triangle.frag");

  // create then install vertex & fragment shaders on GPU
  GLuint program = create_shaders(source_vertex, source_fragment);
  glUseProgram(program);

  // setup imgui context & glfw/opengl backends
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 130");

  // main loop
  while (!glfwWindowShouldClose(window)) {
    // start imgui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // imgui window with widgets
    ImGui::Begin("Hello world");
    ImGui::Text("This is a text");
    ImGui::End();
    ImGui::Render();

    // clear buffer with blue color
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw triangle from bound buffer & imgui window
    glDrawArrays(GL_TRIANGLES, 0, 3);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // process events & show rendered buffer
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  // destroy imgui
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  // destroy shaders, window & terminate glfw
  glDeleteProgram(program);
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
