#ifndef MODEL_ENTITY_HPP
#define MODEL_ENTITY_HPP

#include "render/model_renderer.hpp"

/* Encapsulates 3D model with its renderer for gun */
class Model {
private:
  // program declared first (i.e. init first) as it's needed by renderer
  // https://stackoverflow.com/a/41886567/2228912
  Program m_program;
  AssimpUtil::Model m_model;

  /* Renderer for 3d model */
  ModelRenderer m_renderer;

public:
  Model(Assimp::Importer& importer, const std::string& path, const Program& program, const std::vector<Attribute>& attributes);
  void draw(const Uniforms& uniforms={});
  void set_transform(const Transformation& t);
  void free();
};

#endif // MODEL_ENTITY_HPP
