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

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

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

static GLuint create_shader(const std::string& source_shader, GLenum type_shader) {
  // The Cherno: https://www.youtube.com/watch?v=71BLZwRGUJE
  const char* source_shader_char = source_shader.c_str();

  // compile shader
  GLuint shader = glCreateShader(type_shader);
  glShaderSource(shader, 1, &source_shader_char, NULL);
  glCompileShader(shader);

  // error handling for shader compilation
  GLint result;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    GLint length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    std::vector<GLchar> message(length);
    glGetShaderInfoLog(shader, length, NULL, message.data());
    std::string type_shader_str = (type_shader == GL_VERTEX_SHADER ? "vertex" : "fragment");
    std::cout << "Shader " << type_shader_str << ": " << message.data() << "\n";

    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

static GLuint create_shaders_program(const std::string& source_vertex, const std::string& source_fragment) {
  // create vertex & fragment shaders
  GLuint shader_vertex = create_shader(source_vertex, GL_VERTEX_SHADER);
  GLuint shader_fragment = create_shader(source_fragment, GL_FRAGMENT_SHADER);
  if (shader_vertex == 0 || shader_fragment == 0)
    return 0;

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

  // get monitor size
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);

  // create window and OpenGL context
  GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "OpenGL test", NULL, NULL);
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

  // GPU buffer (VBO) for vertexes (positions, colors, texture coord), see https://open.gl
  const float vertexes[28] = {
     0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top corner
    -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // left corner
     0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // right corner
     0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f// bottom corner
  };
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertexes), vertexes, GL_STATIC_DRAW);

  // GPU buffer for vertexes indices (EBO) to avoid duplication of vertexes
  const GLuint indices[6] = {
    0, 1, 2, // top triangle
    1, 2, 3  // bottom triangle
  };
  GLuint ebo;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // shader source codes (newer GLSL version not supported)
  std::string source_vertex = read_file("assets/shaders/triangle.vert");
  std::string source_fragment = read_file("assets/shaders/triangle.frag");

  // create then install vertex & fragment shaders on GPU
  GLuint program = create_shaders_program(source_vertex, source_fragment);
  if (program == 0) {
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }
  glUseProgram(program);

  // position attribute
  GLuint attr_position = glGetAttribLocation(program, "position");
  glVertexAttribPointer(attr_position, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *) 0);
  glEnableVertexAttribArray(attr_position);

  // color attribute
  GLuint attr_color = glGetAttribLocation(program, "color");
  glVertexAttribPointer(attr_color, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *) (2 * sizeof(float)));
  glEnableVertexAttribArray(attr_color);

  // texture coordinate attribute
  GLuint attr_texture_coord = glGetAttribLocation(program, "texture_coord");
  glVertexAttribPointer(attr_texture_coord, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *) (5 * sizeof(float)));
  glEnableVertexAttribArray(attr_texture_coord);

  // texture
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  // load image into texture
  int width, height, n_channels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* image = stbi_load("assets/images/building.jpg", &width, &height, &n_channels, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  stbi_image_free(image);

  // wrapping texture on mesh
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

    // draw triangle from bound buffers (vbo and ebo) & imgui window
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
  glDeleteTextures(1, &texture);

  // destroy shaders, window & terminate glfw
  glDeleteProgram(program);
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
