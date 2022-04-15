#include "models/mesh.hpp"

using namespace AssimpUtil;

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

/* Set mesh vertexes consisting of positions, normals, uv texture coord, and tangent (i.e. local x-axis) */
void Mesh::set_vertexes() {
  aiVector3D* xyz_coords = m_mesh->mVertices;
  aiVector3D* normals_coords = m_mesh->mNormals;
  aiVector3D* texture_coords = m_mesh->mTextureCoords[0];
  aiVector3D* tangents_coords = m_mesh->mTangents;

  unsigned int n_vertexes = m_mesh->mNumVertices;
  const unsigned int n_coords_position = 3;
  const unsigned int n_coords_normal = 3;
  const unsigned int n_coords_texture = 2;
  const unsigned int n_coords_tangent = 3;

  // not all meshes have normals (also tangents computing requires normals) & texture coordinates
  unsigned int n_coords_vertex = n_coords_position;
  n_coords_vertex += (normals_coords != NULL) ? n_coords_normal : 0;
  n_coords_vertex += (texture_coords != NULL) ? n_coords_texture : 0;
  n_coords_vertex += (tangents_coords != NULL) ? n_coords_tangent : 0;

  vertexes.resize(n_vertexes * n_coords_vertex);
  positions.resize(n_vertexes);

  for (size_t i_vertex = 0; i_vertex < n_vertexes; ++i_vertex) {
    std::vector<float> vertex = { xyz_coords[i_vertex].x, xyz_coords[i_vertex].y, xyz_coords[i_vertex].z };

    // not all meshes have normals (also tangents computing requires normals) & texture coordinates
    if (normals_coords != NULL)
      vertex.insert(vertex.end(), { normals_coords[i_vertex].x, normals_coords[i_vertex].y, normals_coords[i_vertex].z });
    if (texture_coords != NULL)
      vertex.insert(vertex.end(), { texture_coords[i_vertex].x, texture_coords[i_vertex].y });
    if (tangents_coords != NULL)
      vertex.insert(vertex.end(), { tangents_coords[i_vertex].x, tangents_coords[i_vertex].y, tangents_coords[i_vertex].z });

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
