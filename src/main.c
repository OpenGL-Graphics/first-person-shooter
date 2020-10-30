#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void on_key(GLFWwindow* window, int key, int scancode, int action, int mods) {
  // close window on escape key press
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

int main() {
  // initialize glfw
  if (!glfwInit()) {
    printf("Failed to initialize GLFW");
    return 1;
  }

  // create window and OpenGL context
  GLFWwindow* window = glfwCreateWindow(640, 480, "Window title", NULL, NULL);
  if (!window) {
    printf("Failed to create window or OpenGL context");
    return 1;
  }

  // initialize glad before calling gl functions
  glfwMakeContextCurrent(window);
  if (!gladLoadGL()) {
    printf("Failed to load Glad");
    return 1;
  }

  // callback for processing key press
  glfwSetKeyCallback(window, on_key);

  // main loop
  while (!glfwWindowShouldClose(window)) {
    // clear buffer with blue color
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // process events & show rendered buffer
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  // destroy window & terminate glfw
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
