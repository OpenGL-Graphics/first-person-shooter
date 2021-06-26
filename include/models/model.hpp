#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <string>
#include <assimp/scene.h>

#include "models/mesh.hpp"

/* Wrapper struct around Assimp::aiScene class */
struct Model {
  std::vector<float> vertexes;
  std::vector<unsigned int> indices;

  Model(const std::string& path);

private:
  const aiScene* m_scene;
  std::vector<Mesh> m_meshes;
};

#endif // MODEL_HPP
