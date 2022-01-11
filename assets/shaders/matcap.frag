#version 130

in vec3 normal_vert;

/* don't call it `texture2d` as uniform of same name passed in `ModelRenderer::draw()` */
uniform sampler2D texture_matcap;

// world coord  -> camera coord
uniform mat4 view;

out vec4 color_out;

/* Downloaded from: http://mua.github.io/matcap-webgl.html */
void main() {
  // uv-texture from normal in camera space
  vec2 texture_coord = vec2(view * vec4(normalize(normal_vert), 0.0));

  // convert normal from [-1, 1] to [0, 1]
  texture_coord = 0.5 * normalize(texture_coord) + vec2(0.5, 0.5);

  // use normal as uv-coords to sample from texture
  color_out = texture(texture_matcap, texture_coord);
  // color_out = vec4(texture_coord, 0, 1); // intensity depends directly on normal
}
