#include "models/mesh.hpp"

/* Needed to instantiate vector `models::Model::m_meshes` */
Mesh::Mesh() {
}

Mesh::Mesh(aiMesh* mesh):
  m_mesh(mesh)
{
  set_vertexes();
  set_indices();
  material = m_mesh->mMaterialIndex;
}

/* Set mesh vertexes consisting of positions, normals, and texture coord */
void Mesh::set_vertexes() {
  aiVector3D* xyz_coords = m_mesh->mVertices;
  aiVector3D* normals_coords = m_mesh->mNormals;
  aiVector3D* texture_coords = m_mesh->mTextureCoords[0];

  unsigned int n_vertexes = m_mesh->mNumVertices;
  unsigned int n_coords_position = 3;
  unsigned int n_coords_normal = 3;
  unsigned int n_coords_texture = 2;
  unsigned int n_coords_vertex = n_coords_position + n_coords_normal + n_coords_texture;
  vertexes.resize(n_vertexes * n_coords_vertex);
  positions.resize(n_vertexes);

  for (size_t i_vertex = 0; i_vertex < n_vertexes; ++i_vertex) {
    std::vector<float> vertex = {
      xyz_coords[i_vertex].x, xyz_coords[i_vertex].y, xyz_coords[i_vertex].z,
      normals_coords[i_vertex].x, normals_coords[i_vertex].y, normals_coords[i_vertex].z,
      texture_coords[i_vertex].x, texture_coords[i_vertex].y
    };
    std::copy(vertex.begin(), vertex.end(), vertexes.begin() + i_vertex*n_coords_vertex);
    positions[i_vertex] = glm::vec3(xyz_coords[i_vertex].x, xyz_coords[i_vertex].y, xyz_coords[i_vertex].z);
  }
}

/* Set mesh faces (triangles formed by vertexes indices) */
void Mesh::set_indices() {
  unsigned int n_faces = m_mesh->mNumFaces;
  indices.resize(3 * n_faces);

  for (size_t i_face = 0; i_face < n_faces; ++i_face) {
    aiFace face = m_mesh->mFaces[i_face];
    unsigned int n_indices = face.mNumIndices;

    for (size_t i_indice = 0; i_indice < n_indices; ++i_indice) {
      indices[i_face*n_indices + i_indice] = face.mIndices[i_indice];
    }
  }
}
