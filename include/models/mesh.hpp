#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <glm/glm.hpp>
#include <assimp/mesh.h>

#include "texture/texture_2d.hpp"
#include "shader/uniforms.hpp"

/**
 * Wrapper around Assimp::aiMesh used to get vertexes & faces for given mesh
 * Used by `models::Model` to parse meshes within current scene
 * Accessed publicly from `renderer::ModelRenderer` through `models::Model`
 * Namespace to avoid confusion between `models/Model` & `entities/Model`
 */
namespace assimp_utils {
  struct Mesh {
    std::vector<float> vertexes;
    std::vector<unsigned int> indices;

    /* used to calculate bounding box for 3d model (for collision detection) */
    std::vector<glm::vec3> positions;

    unsigned int material;
    glm::vec3 color;
    Texture2D texture_diffuse;
    Texture2D texture_normal;

    /* differentiates meshes with color instead of diffuse texture (e.g. tree) in frag. shader */
    bool has_texture_diffuse;
    bool has_texture_normal;

    /* Default constructor needed by std::vector::resize() (`= default` => ctor defined by compiler) */
    Mesh() = default;
    Mesh(aiMesh* mesh);
    void set_uniforms(Uniforms& uniforms);

  private:
    aiMesh* m_mesh;
    void set_vertexes();
    void set_indices();
  };
}

#endif // MESH_HPP
