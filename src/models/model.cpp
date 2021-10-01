#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "models/model.hpp"
#include "materials/texture.hpp"

using namespace AssimpUtil;

/**
 * Load 3d model in .obj ascii format with Assimp
 * @param path
 * @param importer Scene will be destroyed upon destruction of importer
 */
Model::Model(const std::string& path, Assimp::Importer& importer):
  m_path(path),
  m_directory(m_path.substr(0, m_path.find_last_of('/')))
{
  if (!load_scene(importer)) {
    std::cout << "Failed to load scene from 3D model" << '\n';
    return;
  }

  // append each mesh to use for render later
  unsigned int n_meshes = m_scene->mNumMeshes;
  meshes.resize(n_meshes);
  std::cout << "Parsing " << n_meshes << " meshes..." << '\n';

  for (size_t i_mesh = 0; i_mesh < n_meshes; ++i_mesh) {
    // extract vertexes & indices from each mesh
    meshes[i_mesh] = Mesh(m_scene->mMeshes[i_mesh]);
    aiMaterial* material = m_scene->mMaterials[meshes[i_mesh].material];

    // assign material's diffuse color to mesh
    set_mesh_color(material, i_mesh);

    // assign first material's diffuse texture to mesh
    set_mesh_texture(material, i_mesh);
  }
}

/**
 * Set mesh color from material's diffuse color
 *
 * @param material Material to extract color from
 * @param index Array position of mesh
 */
void Model::set_mesh_color(aiMaterial* material, unsigned int index) {
  aiColor3D color;
  material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
  meshes[index].color = {color.r, color.g, color.b};
}

/**
 * Set mesh texture from material's diffuse texture image
 *
 * @param material Material to extract texture from
 * @param index Array position of mesh
 */
void Model::set_mesh_texture(aiMaterial* material, unsigned int index) {
  unsigned int n_textures_diffuse = material->GetTextureCount(aiTextureType_DIFFUSE);
  std::cout << "n_textures_diffuse: " << n_textures_diffuse << '\n';

  // get path to image used in first diffuse texture
  aiString filename_image;
  unsigned int index_texture = 0;
  material->GetTexture(aiTextureType_DIFFUSE, index_texture, &filename_image);
  std::cout << "n_textures_diffuse: " << n_textures_diffuse << " - path_texture: " << filename_image.C_Str() << '\n';

  // load image only if wasn't loaded before
  std::string path_image = m_directory + "/" + filename_image.C_Str();
  auto it_texture = m_textures_loaded.find(path_image);
  if (it_texture != m_textures_loaded.end()) {
    meshes[index].texture = it_texture->second;
    return;
  }

  // texture from image
  Texture2D texture(Image{path_image});
  m_textures_loaded[path_image] = texture;
  meshes[index].texture = texture;
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
