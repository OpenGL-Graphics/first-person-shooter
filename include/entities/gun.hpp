#ifndef GUN_HPP
#define GUN_HPP

#include "render/model_renderer.hpp"

/* Encapsulates 3D model for gun & stick it at bottom of screen */
class Gun {
private:
  // program declared first (i.e. init first) as it's needed by renderer
  // https://stackoverflow.com/a/41886567/2228912
  Program m_program;
  Model m_model;
  ModelRenderer m_renderer;

public:
  Gun(Assimp::Importer& importer);
  void draw(const Uniforms& uniforms={});
  void set_transform(const Transformation& t);
  void free();
};

#endif // GUN_HPP
