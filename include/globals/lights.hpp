#ifndef LIGHTS_HPP
#define LIGHTS_HPP

#include <glm/glm.hpp>
#include <array>

#include "render/light.hpp"

// lights declared as a global variable (i.e. same instance accessible everywhere)
// https://stackoverflow.com/a/3627979/2228912 
extern std::array<Light, 3> lights;

#endif // LIGHTS_HPP
