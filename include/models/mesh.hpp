#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <glm/glm.hpp>
#include <assimp/mesh.h>

/**
 * Wrapper around Assimp::aiMesh used to get vertexes & faces for given mesh
 * Used by `models::Model` to parse meshes within current scene
 */
struct Mesh {
  std::vector<float> vertexes;
  std::vector<unsigned int> indices;

  Mesh();
  Mesh(aiMesh* mesh);

private:
  aiMesh* m_mesh;
  void set_vertexes();
  void set_indices();
};

#endif // MESH_HPP