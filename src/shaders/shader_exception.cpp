#include "shaders/shader_exception.hpp"

ShaderException::ShaderException():
  m_message("Vertex or fragment shaders failed to compile")
{
}

const char* ShaderException::what() const noexcept {
  return m_message.c_str();
}
