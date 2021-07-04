#include <iostream>

#include "gui/window.hpp"

/* Static class members require a declaration in *.cpp (to allocate space for them) */
Camera* Window::m_camera;
int Window::m_xmouse;
int Window::m_ymouse;

/* Create window & initilialize its OpenGL context */
Window::Window(Camera* camera) {
  // initialize glfw library
  if (!glfwInit()) {
    m_window = NULL;
    std::cout << "Failed to initialize GLFW" << "\n";
    return;
  }

  // windowed mode covering all monitor screen
  Monitor monitor;
  width = monitor.width;
  height = monitor.height;
  m_window = glfwCreateWindow(width, height, "OpenGL test", NULL, NULL);

  // init static members: initial mouse's xy-coords at center of screen
  m_xmouse = monitor.width / 2.0f;
  m_ymouse = monitor.height / 2.0f;
  m_camera = camera;
}

/* Whether window failed to be created */
bool Window::is_null() {
  return m_window == NULL;
}

/* Whether window should be created (by leaving mainloop) */
bool Window::is_closed() {
  return glfwWindowShouldClose(m_window);
}

/* Make window's OpenGL context current to draw on it */
void Window::make_context() {
  glfwMakeContextCurrent(m_window);
}

/* Swap front and back buffers when rendering with OpenGL */
void Window::render() {
  glfwSwapBuffers(m_window);
}

/* Process events by calling associated callbacks */
void Window::process_events() {
  glfwPollEvents();
}

/* Destroy window & its context and free its resources */
void Window::destroy() {
  glfwDestroyWindow(m_window);
  glfwTerminate();
}

/* Listener for keypress called on every frame of mainloop */
void Window::on_keypress() {
  // close window on escape key press (by setting flag & check if set in `is_closed()`)
  if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
  }

  // move camera
  if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
    m_camera->move(Direction::FORWARD);
  if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
    m_camera->move(Direction::BACKWARD);
  if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
    m_camera->move(Direction::LEFT);
  if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
    m_camera->move(Direction::RIGHT);
}

void Window::attach_listeners() {
  glfwSetMouseButtonCallback(m_window, on_mouse_click);
}

void Window::on_mouse_move(GLFWwindow* window, double xpos, double ypos) {
  // see: https://www.reddit.com/r/opengl/comments/831vpb/
  // calculate offset in mouse cursor position
  float x_offset = xpos - m_xmouse;
  float y_offset = ypos - m_ymouse;
  m_xmouse = xpos;
  m_ymouse = ypos;

  // horizontal/vertical rotation around y-axis/x-axis accord. to offset
  m_camera->rotate(x_offset, y_offset);
}

/* Zoom in/out using mouse wheel */
void Window::on_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset) {
  if (yoffset == 1) {
    m_camera->zoom(Zoom::IN);
  } else if (yoffset == -1) {
    m_camera->zoom(Zoom::OUT);
  }
}

/* Switch mouse mode & listen for mouse movement/scroll */
void Window::on_mouse_click(GLFWwindow* window, int button, int action, int mods) {
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
