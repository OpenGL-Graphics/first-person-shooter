#ifndef CUBE_HPP
# define CUBE_HPP

#include <glad/glad.h>
#include <string>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

class Cube {
public:
  Cube(GLuint program, const std::vector<std::string>& names_attributes, const std::vector<std::string>& paths_textures) {
    // load cube vertexes in buffer & get attributes from shaders & apply 6-sided texture from image
    init_buffer();
    define_attributes(program, names_attributes);
    apply_texture(paths_textures);
  }

  void draw() {
    // draw all vertexes from cube triangles from bound buffers (vbo)
    glDrawArrays(GL_TRIANGLES, 0, 6 * 2 * 3);
  }

  void free() {
    // de-allocate GPU buffers & textures
    glDeleteBuffers(1, &m_vbo);
    glDeleteTextures(1, &m_texture);
  }

private:
  GLuint m_vbo;
  GLuint m_texture;

  // coord(x,y,z)        color(r,g,b)      texture(u,v,w)
  const float VERTEXES[6 * 6 * 9] = {
    // negative-x (left face)
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, -1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -1.0f,  1.0f, -1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f,  1.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, -1.0f,  1.0f,  1.0f,

     // positive-x (right face)
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f,  1.0f,  1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f,  1.0f, -1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,  1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f,  1.0f,  1.0f,

     // negative-y (bottom face)
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  1.0f, -1.0f, -1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  1.0f, -1.0f,  1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  1.0f, -1.0f,  1.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f,

    // positive-y (top face)
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f, -1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,  1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,  1.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f,

    // negative-z (back face)
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  1.0f, -1.0f, -1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  1.0f,  1.0f, -1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  1.0f,  1.0f, -1.0f, 
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -1.0f,  1.0f, -1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f,

    // positive-z (front face)
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  1.0f, -1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  1.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, -1.0f,  1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,
  };

  void init_buffer() {
    // GPU buffer (VBO) for vertexes (positions, colors, texture direction), see https://open.gl
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEXES), VERTEXES, GL_STATIC_DRAW);
  }

  void define_attributes(GLuint program, const std::vector<std::string>& names_attributes) {
    // position attribute
    GLuint attr_position = glGetAttribLocation(program, names_attributes[0].c_str());
    glVertexAttribPointer(attr_position, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(attr_position);

    // color attribute
    GLuint attr_color = glGetAttribLocation(program, names_attributes[1].c_str());
    glVertexAttribPointer(attr_color, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(attr_color);

    // texture direction attribute
    GLuint attr_texture_dir = glGetAttribLocation(program, names_attributes[2].c_str());
    glVertexAttribPointer(attr_texture_dir, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(attr_texture_dir);
  }

  void apply_texture(const std::vector<std::string>& paths_textures) {
    // texture
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);

    // 6-sided texture cube using loaded images 
    for (size_t i_texture = 0; i_texture < 6; i_texture++) {
      // stbi_set_flip_vertically_on_load(true);
      int width_image, height_image, n_channels_image;
      unsigned char* image = stbi_load(paths_textures[i_texture].c_str(), &width_image, &height_image, &n_channels_image, 0);
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
};

#endif // CUBE_HPP
