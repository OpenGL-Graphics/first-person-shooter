#include <gui/dialog.hpp>

Dialog::Dialog(GLFWwindow* window, const std::string& title, const std::string& text):
  m_title(title),
  m_text(text)
{
  // setup imgui context & glfw/opengl backends
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 130");
}

void Dialog::render() {
  // start imgui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // imgui window with widgets
  ImGui::Begin(m_title.c_str());
  ImGui::Text("%s", m_text.c_str());
  ImGui::End();
  ImGui::Render();

  // render imgui window
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Dialog::free() {
  // destroy imgui
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
