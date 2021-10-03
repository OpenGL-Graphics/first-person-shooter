#ifndef TARGETS_HPP
#define TARGETS_HPP

#include "entities/target.hpp"

/* static class */
class Targets {
public:
  static std::vector<Target> cubes;
  static void add(const Target& target);
  static void free();

private:
  /* Cannot create class instances */
  Targets();
};

#endif // TARGETS_HPP
