#include <iostream>

#include "gui/window.hpp"

/* Create window & initilialize its OpenGL context */
Window::Window(const std::string& title) {
  // initialize glfw library
  if (!glfwInit()) {
    w = NULL;
    std::cout << "Failed to initialize GLFW" << "\n";
    return;
  }

  // window in full-screen mode
  Monitor monitor;
  w = glfwCreateWindow(monitor.width, monitor.height, title.c_str(), monitor.m, NULL);

  // get window size (same as monitor in full-screen mode)
  glfwGetWindowSize(w, &width, &height);
}

/* Whether window failed to be created */
bool Window::is_null() {
  return w == NULL;
}

/* Whether window should be created (by leaving mainloop) */
bool Window::is_closed() {
  return glfwWindowShouldClose(w);
}

/* Close window by setting flag (& leaving mainloop) */
void Window::close() const {
  glfwSetWindowShouldClose(w, GLFW_TRUE);
}

/* Make window's OpenGL context current to draw on it */
void Window::make_context() {
  glfwMakeContextCurrent(w);
}

/* Swap front and back buffers when rendering with OpenGL */
void Window::render() {
  glfwSwapBuffers(w);
}

/* Process events by calling associated callbacks */
void Window::process_events() {
  glfwPollEvents();
}

/* Destroy window & its context and free its resources */
void Window::destroy() {
  glfwDestroyWindow(w);
  glfwTerminate();
}

/* Check if given `key` was pressed */
bool Window::is_key_pressed(int key) const {
  return glfwGetKey(w, key) == GLFW_PRESS;
}

/**
 * Attach mouse move/click/scroll callbacks
 * Listeners implemented inside `MouseHandler`
 */
void Window::attach_mouse_listeners(GLFWcursorposfun on_mouse_move, GLFWmousebuttonfun on_mouse_click, GLFWscrollfun on_mouse_scroll) {
  // hide cursor & unlimited mouse movement
  glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // listen for mouse click/movement/scroll
  glfwSetMouseButtonCallback(w, on_mouse_click);
  glfwSetCursorPosCallback(w, on_mouse_move);
  glfwSetScrollCallback(w, on_mouse_scroll);
}
