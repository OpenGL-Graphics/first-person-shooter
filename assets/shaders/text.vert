#version 130

// defined in freetype-gl's text-buffer
in vec3 vertex;
in vec2 tex_coord;
in vec4 color;
in float shift;
in float gamma;

// opengl tranformation matrices
uniform mat4 model;      // object coord -> world coord
uniform mat4 view;       // world coord  -> camera coord
uniform mat4 projection; // camera coord -> ndc coord

out vec2 tex_coord_vert;
out vec4 color_vert;
out float shift_vert;
out float gamma_vert;

void main() {
  gl_Position = projection * view * model * vec4(vertex, 1.0);

  // pass to fragment
  tex_coord_vert = tex_coord;
  color_vert = color;
  shift_vert = shift;
  gamma_vert = gamma;
}
