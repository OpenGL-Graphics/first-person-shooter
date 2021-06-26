#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <string>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include "models/mesh.hpp"

/**
 * Wrapper struct around Assimp::aiScene class
 * Serves as parent of all meshes inside scene (i.e. 3D model in *.obj format)
 */
struct Model {
  std::vector<Mesh> meshes;

  Model(const std::string& path, Assimp::Importer& importer);

private:
  const aiScene* m_scene;
  std::string m_path;

  bool load_scene(Assimp::Importer& importer);
};

#endif // MODEL_HPP
