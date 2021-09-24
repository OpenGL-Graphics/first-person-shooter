#ifndef SHADER_EXCEPTION_HPP
#define SHADER_EXCEPTION_HPP

#include <exception>
#include <string>

class ShaderException : public std::exception {
  public:
    ShaderException();
    const char* what() const noexcept override;

  private:
    std::string m_message;
};

#endif // SHADER_EXCEPTION_HPP
