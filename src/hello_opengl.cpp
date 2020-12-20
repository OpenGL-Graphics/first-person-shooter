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
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

// camera
glm::vec3 pos_camera(0.0f, 0.0f, 5.0f);
glm::vec3 dir_camera(0.0f, 0.0f, -1.0f);
glm::vec3 up_camera(0.0f, 1.0f, 0.0f);
const float X_SPEED = 0.1f;
const float Z_SPEED = 0.1f;

static std::string read_file(const std::string& filename) {
  std::ifstream f(filename.c_str());
  std::stringstream buffer;
  buffer << f.rdbuf();

  return buffer.str();
}

static void on_key(GLFWwindow* window) {
  // close window on escape key press
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  // move camera
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    pos_camera -= Z_SPEED * glm::vec3(0.0f, 0.0f, 1.0f);
  }
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    pos_camera += Z_SPEED * glm::vec3(0.0f, 0.0f, 1.0f);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    pos_camera -= X_SPEED * glm::vec3(1.0f, 0.0f, 0.0f);
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    pos_camera += X_SPEED * glm::vec3(1.0f, 0.0f, 0.0f);
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

  // get monitor width & height
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);
  const int width_monitor = mode->width;
  const int height_monitor = mode->height;

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

  // callback for processing key press
  // glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
  // glfwSetKeyCallback(window, on_key);

  // GPU buffer (VBO) for vertexes (positions, colors, texture direction), see https://open.gl
  // coord(x,y,z)        color(r,g,b)      texture(u,v,w)
  const float vertexes[] = {
    // negative-x (left face)
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, -1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -1.0f,  1.0f, -1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f,  1.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, -1.0f,  1.0f,  1.0f,

     // positive-x (right face)
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f,  1.0f,  1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f,  1.0f, -1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,  1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f,  1.0f,  1.0f,

     // negative-y (bottom face)
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  1.0f, -1.0f, -1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  1.0f, -1.0f,  1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  1.0f, -1.0f,  1.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f,

    // positive-y (top face)
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f, -1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,  1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,  1.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f,

    // negative-z (back face)
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  1.0f, -1.0f, -1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  1.0f,  1.0f, -1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  1.0f,  1.0f, -1.0f, 
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -1.0f,  1.0f, -1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f,

    // positive-z (front face)
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  1.0f, -1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  1.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, -1.0f,  1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,
  };
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertexes), vertexes, GL_STATIC_DRAW);

  /*
  // GPU buffer for vertexes indices (EBO) to avoid duplication of vertexes
  const GLuint indices[6] = {
    0, 1, 2, // top triangle
    1, 2, 3  // bottom triangle
  };
  GLuint ebo;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  */

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
  glVertexAttribPointer(attr_position, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *) 0);
  glEnableVertexAttribArray(attr_position);

  // color attribute
  GLuint attr_color = glGetAttribLocation(program, "color");
  glVertexAttribPointer(attr_color, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *) (3 * sizeof(float)));
  glEnableVertexAttribArray(attr_color);

  // texture direction attribute
  GLuint attr_texture_dir = glGetAttribLocation(program, "texture_dir");
  glVertexAttribPointer(attr_texture_dir, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *) (6 * sizeof(float)));
  glEnableVertexAttribArray(attr_texture_dir);

  // texture
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

  // 6-sided texture cube using loaded images 
  std::string images_names[6] = {
    "assets/images/brick1.jpg",
    "assets/images/brick1.jpg",
    "assets/images/roof.jpg",
    "assets/images/roof.jpg",
    "assets/images/brick2.jpg",
    "assets/images/brick2.jpg",
  };
  for (size_t i_texture = 0; i_texture < 6; i_texture++) {
    // stbi_set_flip_vertically_on_load(true);
    int width_image, height_image, n_channels_image;
    unsigned char* image = stbi_load(images_names[i_texture].c_str(), &width_image, &height_image, &n_channels_image, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i_texture, 0, GL_RGB, width_image, height_image, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
  }

  // wrapping texture on mesh
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // projection matrix (3/3): transform to ndc coord
  glm::mat4 projection_mat = glm::perspective(glm::radians(45.0f), (float)width_monitor/(float)height_monitor, 1.0f, 10.f); 
  GLuint uniform_projection = glGetUniformLocation(program, "projection");
  glUniformMatrix4fv(uniform_projection, 1, GL_FALSE, glm::value_ptr(projection_mat));

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

    /*
    // rotating camera using basic trigonometry
    const float radius = 5.0f;
    float x_camera = radius * cos((float)glfwGetTime());
    float z_camera = radius * sin((float)glfwGetTime());
    */

    // view matrix (2/3): transform to camera coord
    glm::mat4 view_mat = glm::lookAt(pos_camera, pos_camera + dir_camera, up_camera);
    GLuint uniform_view = glGetUniformLocation(program, "view");
    glUniformMatrix4fv(uniform_view, 1, GL_FALSE, glm::value_ptr(view_mat));

    // draw multiple cubes
    for (char i_cube = -2; i_cube <= 2; i_cube += 2) {
      // model matrix (1/3): ever rotating rectangle using time
      glm::mat4 model_mat(1.0f);
      model_mat = glm::translate(model_mat, glm::vec3(i_cube, 0.0f, 0.0f));
      // model_mat = glm::rotate(model_mat, (float)glfwGetTime() * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
      GLuint uniform_model = glGetUniformLocation(program, "model");
      glUniformMatrix4fv(uniform_model, 1, GL_FALSE, glm::value_ptr(model_mat));

      // draw triangle from bound buffers (vbo and ebo)
      // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      glDrawArrays(GL_TRIANGLES, 0, 36);
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
  glDeleteBuffers(1, &vbo);
  // glDeleteBuffers(1, &ebo);
  glDeleteTextures(1, &texture);

  // destroy shaders, window & terminate glfw
  glDeleteProgram(program);
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
