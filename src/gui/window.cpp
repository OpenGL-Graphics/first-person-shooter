#include <iostream>

#include "gui/window.hpp"

/* Create window & initilialize its OpenGL context */
Window::Window(const std::string& title) {
  // initialize glfw library
  if (!glfwInit()) {
    m_window = NULL;
    std::cout << "Failed to initialize GLFW" << "\n";
    return;
  }

  // window in full-screen mode
  Monitor monitor;
  m_window = glfwCreateWindow(monitor.width, monitor.height, title.c_str(), monitor.m, NULL);

  // get window size (same as monitor in full-screen mode)
  glfwGetWindowSize(m_window, &width, &height);
}

/* Whether window failed to be created */
bool Window::is_null() {
  return m_window == NULL;
}

/* Whether window should be created (by leaving mainloop) */
bool Window::is_closed() {
  return glfwWindowShouldClose(m_window);
}

/* Close window by setting flag (& leaving mainloop) */
void Window::close() const {
  glfwSetWindowShouldClose(m_window, GLFW_TRUE);
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

/* Check if given `key` was pressed */
bool Window::is_key_pressed(int key) const {
  return glfwGetKey(m_window, key) == GLFW_PRESS;
}

/**
 * Attach `on_mouse_click` callback on mouse click
 * Used by `MouseHandler`
 */
void Window::attach_mouse_listener(GLFWmousebuttonfun	on_mouse_click) {
  glfwSetMouseButtonCallback(m_window, on_mouse_click);
}
