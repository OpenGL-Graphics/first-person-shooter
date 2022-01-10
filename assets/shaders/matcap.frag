#version 130

in vec3 normal_vert;

uniform sampler2D texture2d;
uniform mat4 view; // world coord  -> camera coord

out vec4 color_out;

/* Downloaded from: http://mua.github.io/matcap-webgl.html */
void main() {
  // uv-texture from normal in camera space
  vec2 texture_coord = vec2(view * vec4(normalize(normal_vert), 0.0));

  // convert normal from [-1, 1] to [0, 1]
  texture_coord = 0.5 * normalize(texture_coord) + vec2(0.5, 0.5);

  color_out = texture(texture2d, texture_coord);
}
