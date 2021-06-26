#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "models/model.hpp"

/**
 * Load 3d model in .obj ascii format with Assimp
 * @param path
 */
Model::Model(const std::string& path) {
  // flags ensures each face has 3 vertexes indices
  Assimp::Importer importer;
  m_scene = importer.ReadFile(path, aiProcess_Triangulate);

  if (m_scene == NULL) {
    std::cout << "Failed to load 3D model" << '\n';
    return;
  }

  // append vertexes & indices from each mesh into model's ones
  unsigned int n_meshes = m_scene->mNumMeshes;
  m_meshes.resize(n_meshes);

  for (size_t i_mesh = 0; i_mesh < n_meshes; ++i_mesh) {
    m_meshes[i_mesh] = Mesh(m_scene->mMeshes[i_mesh]);
    std::vector<float> vertexes_mesh = m_meshes[i_mesh].vertexes;
    std::vector<unsigned int> indices_mesh = m_meshes[i_mesh].indices;

    vertexes.insert(vertexes.end(), vertexes_mesh.begin(), vertexes_mesh.end());
    indices.insert(indices.end(), indices_mesh.begin(), indices_mesh.end());
  }
}
