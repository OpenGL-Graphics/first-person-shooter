#ifndef SHADERS_FACTORY_HPP
#define SHADERS_FACTORY_HPP

#include "program.hpp"

/**
 * Factory to produce singletons shader Programs
 * avoids duplication of shaders declared internally
 */
class ShadersFactory {
public:
  ShadersFactory();
  const Program& operator[](const std::string& key) const;
  bool has_failed();
  void free();

private:
  std::unordered_map<std::string, Program> m_programs;
};

#endif // SHADERS_FACTORY_HPP
