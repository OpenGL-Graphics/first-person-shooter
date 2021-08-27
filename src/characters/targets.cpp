#include "characters/targets.hpp"

Targets::Targets(const std::vector<Renderer *>& renderers):
  m_renderers(renderers)
{
}

void Targets::add(Renderer* renderer) {
  m_renderers.push_back(renderer);
}
