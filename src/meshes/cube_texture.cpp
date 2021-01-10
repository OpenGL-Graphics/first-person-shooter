#include <meshes/cube_texture.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

CubeTexture::CubeTexture(const Program& program, const std::vector<std::string>& paths_images):
  Cube(program)
{
  // get texture attribute from shaders & apply 6-sided texture from image
  set_attribute(program);
  apply_texture(paths_images);
}

void CubeTexture::free() {
  // de-allocate GPU buffers & textures
  Cube::free();
  glDeleteTextures(1, &m_texture);
}

void CubeTexture::set_attribute(const Program& program) {
  // texture direction attribute
  GLuint attr_texture_dir = program.define_attribute("texture_dir");
  glVertexAttribPointer(attr_texture_dir, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void *) (6 * sizeof(float)));
  glEnableVertexAttribArray(attr_texture_dir);
}

void CubeTexture::apply_texture(const std::vector<std::string>& paths_images) {
  // texture
  glGenTextures(1, &m_texture);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);

  // 6-sided texture cube using loaded images 
  for (size_t i_texture = 0; i_texture < 6; i_texture++) {
    // stbi_set_flip_vertically_on_load(true);
    int width_image, height_image, n_channels_image;
    unsigned char* image = stbi_load(paths_images[i_texture].c_str(), &width_image, &height_image, &n_channels_image, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i_texture, 0, GL_RGB, width_image, height_image, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
  }

  // wrapping texture on mesh
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
