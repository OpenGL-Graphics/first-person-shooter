#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <glm/glm.hpp>
#include <assimp/mesh.h>

#include "materials/texture.hpp"

/**
 * Wrapper around Assimp::aiMesh used to get vertexes & faces for given mesh
 * Used by `models::Model` to parse meshes within current scene
 * Accessed publicly from `renderer::ModelRenderer` through `models::Model`
 */
struct Mesh {
  std::vector<float> vertexes;
  std::vector<unsigned int> indices;

  unsigned int material;
  glm::vec3 color;
  Texture2D texture;

  Mesh();
  Mesh(aiMesh* mesh);

private:
  aiMesh* m_mesh;
  void set_vertexes();
  void set_indices();
};

#endif // MESH_HPP
