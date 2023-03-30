/**
 * Cubemap texture:
 *   - 6 x 2D images
 *   - Texture coords: 3D direction vector (u, v, w) from cube center to vertexes
 *   - u, v, w: in [0, 1] with point (0, 0) being lower-left corner,
 *              if outside the range, GL_CLAMP_TO_EDGE can clamp coords in [0, 1]
 * https://www.khronos.org/opengl/wiki/Cubemap_Texture
 * https://learnopengl.com/Advanced-OpenGL/Cubemaps
 */
#include "geometries/cube.hpp"

using namespace geometry;

/**
 * @param is_skybox Skybox has internal triangles (i.e. indices) with CCW winding order (visible)
 *                  => only internal faces are visible with face culling enabled
 */
Cube::Cube(bool is_skybox, const glm::vec3& size):
  m_width(size.x),
  m_height(size.y),
  m_depth(size.z)
{
  // cannot be init in constructor's member initalizer list as they're members of base class
  m_vertexes = get_vertexes();
  m_indices = (is_skybox) ? INDICES_INTERNAL : INDICES_EXTERNAL;
  m_positions = POSITIONS;

  // duplicated vertexes across triangles
  unsigned int n_faces = 6;
  unsigned int n_triangles = 2 * n_faces;
  m_n_elements = 3 * n_triangles;
}

unsigned int Cube::get_n_elements() const {
  return m_n_elements;
}

std::vector<unsigned int> Cube::get_indices() const {
  return m_indices;
}

std::vector<glm::vec3> Cube::get_positions() const {
  return m_positions;
}

/**
 * Duplicated xyz vertexes bcos of different normals
 * Cube centered around origin with baked-in xyz (no scaling needed) & uv-coords (to avoid stretching texture)
 * 2d uv-texture coords used instead of 3d texture (cube maps), as the latter didn't repeat
 *  coord(x,y,z)                          normal     uv
 */
std::vector<float> Cube::get_vertexes() const {
  return {
    // negative-x (left face)
    -m_width/2,  m_height/2,  m_depth/2, -1,  0,  0, m_depth, m_height,
    -m_width/2,  m_height/2, -m_depth/2, -1,  0,  0, 0,       m_height,
    -m_width/2, -m_height/2, -m_depth/2, -1,  0,  0, 0,       0,
    -m_width/2, -m_height/2,  m_depth/2, -1,  0,  0, m_depth, 0,

    // positive-x (right face)
     m_width/2,  m_height/2,  m_depth/2,  1,  0,  0, m_depth, m_height,
     m_width/2,  m_height/2, -m_depth/2,  1,  0,  0, 0,       m_height,
     m_width/2, -m_height/2, -m_depth/2,  1,  0,  0, 0,       0,
     m_width/2, -m_height/2,  m_depth/2,  1,  0,  0, m_depth, 0,

    // negative-y (bottom face)
    -m_width/2, -m_height/2, -m_depth/2,  0, -1,  0, 0,       0,
     m_width/2, -m_height/2, -m_depth/2,  0, -1,  0, m_width, 0,
     m_width/2, -m_height/2,  m_depth/2,  0, -1,  0, m_width, m_depth,
    -m_width/2, -m_height/2,  m_depth/2,  0, -1,  0, 0,       m_depth,

    // positive-y (top face)
    -m_width/2,  m_height/2, -m_depth/2,  0,  1,  0, 0,       0,
     m_width/2,  m_height/2, -m_depth/2,  0,  1,  0, m_width, 0,
     m_width/2,  m_height/2,  m_depth/2,  0,  1,  0, m_width, m_depth,
    -m_width/2,  m_height/2,  m_depth/2,  0,  1,  0, 0,       m_depth,

    // negative-z (back face)
    -m_width/2, -m_height/2, -m_depth/2,  0,  0, -1, 0,       0,
     m_width/2, -m_height/2, -m_depth/2,  0,  0, -1, m_width, 0,
     m_width/2,  m_height/2, -m_depth/2,  0,  0, -1, m_width, m_height,
    -m_width/2,  m_height/2, -m_depth/2,  0,  0, -1, 0,       m_height,

    // positive-z (front face)
    -m_width/2, -m_height/2,  m_depth/2,  0,  0,  1, 0,       0,
     m_width/2, -m_height/2,  m_depth/2,  0,  0,  1, m_width, 0,
     m_width/2,  m_height/2,  m_depth/2,  0,  0,  1, m_width, m_height,
    -m_width/2,  m_height/2,  m_depth/2,  0,  0,  1, 0,       m_height,
  };
}

/* For cubes with external faces visible (normal pointing outwards) */
const std::vector<unsigned int> Cube::INDICES_EXTERNAL {
  // negative-x (left face)
  0, 1, 2,
  2, 3, 0,

  // positive-x (right face)
  4, 6, 5,
  4, 7, 6,

  // negative-y (bottom face)
  8, 9, 10,
  10, 11, 8,

  // positive-y (top face)
  12, 14, 13,
  12, 15, 14,

  // negative-z (back face)
  16, 18, 17,
  16, 19, 18,

  // positive-z (front face)
  20, 21, 22,
  22, 23, 20,
};

/* Skybox with internal faces visible (normal pointing inwards) */
const std::vector<unsigned int> Cube::INDICES_INTERNAL {
  // negative-x (left face)
  0, 2, 1,
  2, 0, 3,

  // positive-x (right face)
  4, 5, 6,
  4, 6, 7,

  // negative-y (bottom face)
  8, 10, 9,
  10, 8, 11,

  // positive-y (top face)
  12, 13, 14,
  12, 14, 15,

  // negative-z (back face)
  16, 17, 18,
  16, 18, 19,

  // positive-z (front face)
  20, 22, 21,
  22, 20, 23,
};

// TODO: Is it not the same as VERTEXES?
// copy of vertexes above containing only (x, y, z) coords
const std::vector<glm::vec3> Cube::POSITIONS = {
  // negative-x (left face)
  {-0.5f,  0.5f,  0.5f},
  {-0.5f,  0.5f, -0.5f},
  {-0.5f, -0.5f, -0.5f},
  {-0.5f, -0.5f,  0.5f},

  // positive-x (right face)
  {0.5f,  0.5f,  0.5f},
  {0.5f,  0.5f, -0.5f},
  {0.5f, -0.5f, -0.5f},
  {0.5f, -0.5f,  0.5f},

  // negative-y (bottom face)
  {-0.5f, -0.5f, -0.5f},
  { 0.5f, -0.5f, -0.5f},
  { 0.5f, -0.5f,  0.5f},
  {-0.5f, -0.5f,  0.5f},

  // positive-y (top face)
  {-0.5f,  0.5f, -0.5f},
  { 0.5f,  0.5f, -0.5f},
  { 0.5f,  0.5f,  0.5f},
  {-0.5f,  0.5f,  0.5f},

  // negative-z (back face)
  {-0.5f, -0.5f, -0.5f},
  { 0.5f, -0.5f, -0.5f},
  { 0.5f,  0.5f, -0.5f},
  {-0.5f,  0.5f, -0.5f},

  // positive-z (front face)
  {-0.5f, -0.5f,  0.5f},
  { 0.5f, -0.5f,  0.5f},
  { 0.5f,  0.5f,  0.5f},
  {-0.5f,  0.5f,  0.5f},
};
