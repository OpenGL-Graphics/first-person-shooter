#version 130

in vec3 position;
in vec3 normal;
in vec2 texture_coord;

// opengl tranformation matrices
uniform mat4 model;      // object coord -> world coord
uniform mat4 view;       // world coord  -> camera coord
uniform mat4 projection; // camera coord -> ndc coord

out vec3 position_vert;
out vec3 normal_vert;
out vec2 texture_coord_vert;

void main() {
  // vertical wave shape using sin function
  // xz supposed in [0, 1]
  // TODO: shading doesn't work, as normals weren't precalculated (normal=y-axis always)
  const float PI = 3.141592;
  vec3 position_wave = position;
  position_wave.y = sin(2*PI * position.x);
  gl_Position = projection * view * model * vec4(position_wave, 1.0);

  // fragment position is in world space (same as light)
  position_vert = (model * vec4(position_wave, 1.0)).xyz;
  normal_vert = normal;
  texture_coord_vert = texture_coord;
}
