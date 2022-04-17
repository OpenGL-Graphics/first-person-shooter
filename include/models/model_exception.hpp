#ifndef MODEL_EXCEPTION_HPP
#define MODEL_EXCEPTION_HPP

#include <exception>
#include <string>

class ModelException : public std::exception {
  public:
    ModelException();
    const char* what() const noexcept override;

  private:
    std::string m_message;
};

#endif // MODEL_EXCEPTION_HPP
