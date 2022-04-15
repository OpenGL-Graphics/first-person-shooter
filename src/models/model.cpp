#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "models/model.hpp"
#include "texture.hpp"

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

    // assign first material's diffuse & normal texture to mesh
    set_mesh_texture(material, i_mesh, aiTextureType_DIFFUSE);
    set_mesh_texture(material, i_mesh, aiTextureType_HEIGHT);
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
 * Set mesh texture from material's diffuse texture image (textures freed in `ModelRenderer`)
 *
 * @param material Material to extract texture from
 * @param index Array position of mesh
 * @param type Texture type (diffuse, normal...)
 */
void Model::set_mesh_texture(aiMaterial* material, unsigned int index, aiTextureType type) {
  // get path to image used in first diffuse texture
  unsigned int n_textures_diffuse = material->GetTextureCount(type);
  aiString filename_image;
  unsigned int index_texture = 0;
  material->GetTexture(type, index_texture, &filename_image);
  std::cout << "n_textures: " << n_textures_diffuse << " - filename texture: " << filename_image.C_Str() << '\n';

  // texture image already loaded before (meshes can share same texture)
  std::string path_image = m_directory + "/" + filename_image.C_Str();
  auto it_texture = m_textures_loaded.find(path_image);
  if (it_texture != m_textures_loaded.end()) {
    if (type == aiTextureType_DIFFUSE)
      meshes[index].texture_diffuse = it_texture->second;
    else if (type == aiTextureType_HEIGHT)
      meshes[index].texture_normal = it_texture->second;
    return;
  }

  // load texture from image
  GLenum texture_unit = (type == aiTextureType_DIFFUSE) ? GL_TEXTURE0 : GL_TEXTURE1;
  Texture2D texture(Image{path_image}, texture_unit);
  m_textures_loaded[path_image] = texture;

  if (type == aiTextureType_DIFFUSE)
    meshes[index].texture_diffuse = texture;
  else if (type == aiTextureType_HEIGHT)
    meshes[index].texture_normal = texture;
}

/**
 * Load scene by path
 * @return true on success, false on failure
 */
bool Model::load_scene(Assimp::Importer& importer) {
  // flags ensures each face has 3 vertexes indices
  // calculate TBN matrix for normal mapping: https://learnopengl.com/Advanced-Lighting/Normal-Mapping
  m_scene = importer.ReadFile(m_path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

  if (m_scene == NULL) {
    return false;
  }

  return true;
}

/* Free meshes textures (diffuse & normal) */
void Model::free() {
  for (const auto& pair : m_textures_loaded) {
    pair.second.free();
  }
}
