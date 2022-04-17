#include "globals/targets.hpp"

/* Static class members require a declaration in *.cpp (to allocate space for them & avoid linking error) */
std::vector<Target> Targets::samurais;

void Targets::add(const Target& target) {
  samurais.push_back(target);
}

/* Free all targets */
void Targets::free() {
  for (Target& target : Targets::samurais) {
    target.free();
  }
}
