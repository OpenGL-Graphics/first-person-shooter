#ifndef TARGETS_HPP
#define TARGETS_HPP

#include "render/renderer.hpp"

/* Targets to destroy on intersection with mouse cursor */
class Targets {
public:
  Targets(const std::vector<Renderer *>& renderers);
  void add(Renderer* renderer);

private:
  std::vector<Renderer *> m_renderers;
};

#endif // TARGETS_HPP
