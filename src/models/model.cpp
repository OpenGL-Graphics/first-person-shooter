#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "models/model.hpp"

/**
 * Load 3d model in .obj ascii format with Assimp
 * @param path
 * @param importer Scene will be destroyed upon destruction of importer
 */
Model::Model(const std::string& path, Assimp::Importer& importer):
  m_path(path)
{
  if (!load_scene(importer)) {
    std::cout << "Failed to load scene from 3D model" << '\n';
    return;
  }

  // append each mesh to use for render later
  unsigned int n_meshes = m_scene->mNumMeshes;
  meshes.resize(n_meshes);

  for (size_t i_mesh = 0; i_mesh < n_meshes; ++i_mesh) {
    // extract vertexes & indices from each mesh
    meshes[i_mesh] = Mesh(m_scene->mMeshes[i_mesh]);

    // assign used material's diffuse color to mesh
    aiMaterial* material = m_scene->mMaterials[meshes[i_mesh].material];
    aiColor3D color;
    material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    meshes[i_mesh].color = {color.r, color.g, color.b};
    std::cout << "Color mesh: " << color.r << " " << color.g << " " << color.b << '\n';

    /*
    // get diffuse textures from each mesh material
    unsigned int n_textures_diffuse = material->GetTextureCount(aiTextureType_DIFFUSE);
    std::cout << "n_textures_diffuse: " << n_textures_diffuse << '\n';
    for (size_t i_texture = 0; i_texture < n_textures_diffuse; ++i_texture) {
      // get path to texture image
      aiString path_texture;
      material->GetTexture(aiTextureType_DIFFUSE, i_texture, &path_texture);
      std::cout << "n_textures_diffuse: " << n_textures_diffuse << " - path_texture: " << path_texture.C_Str() << '\n';
    }
    */
  }
}

/**
 * Load scene by path
 * @return true on success, false on failure
 */
bool Model::load_scene(Assimp::Importer& importer) {
  // flags ensures each face has 3 vertexes indices
  m_scene = importer.ReadFile(m_path, aiProcess_Triangulate);

  if (m_scene == NULL) {
    return false;
  }

  return true;
}
