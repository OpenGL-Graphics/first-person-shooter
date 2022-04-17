#include "models/model_exception.hpp"

ModelException::ModelException():
  m_message("Failed to load scene from 3D model (make sure file path is correct)")
{
}

const char* ModelException::what() const noexcept {
  return m_message.c_str();
}
