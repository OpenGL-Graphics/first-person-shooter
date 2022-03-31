#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coord;

// opengl tranformation matrices
uniform mat4 model;      // object coord -> world coord
uniform mat4 view;       // world coord  -> camera coord
uniform mat4 projection; // camera coord -> ndc coord

// time in sec since GLFW was init (used to animate wave)
uniform float time;

out vec3 position_vert;
out vec3 normal_vert;
out vec2 texture_coord_vert;

void main() {
  // animated wave shape using sin function
  // x supposed in [0, 1] => 2pi*x in [0, 2pi], add time to shift curve/animate (angle = 2k*pi + t)
  // TODO: shading doesn't work, as normals weren't precalculated (normal=y-axis always)
  const float PI = 3.141592;
  vec3 position_wave = position;
  float angle = (2*PI * position.x) + time;
  position_wave.y = sin(angle);
  gl_Position = projection * view * model * vec4(position_wave, 1.0);

  // fragment position is in world space (same as light)
  position_vert = (model * vec4(position_wave, 1.0)).xyz;
  normal_vert = normal;
  texture_coord_vert = texture_coord;
}
