#ifndef TEXTURES_FACTORY_HPP
#define TEXTURES_FACTORY_HPP

#include <memory>
#include <unordered_map>

#include "texture/texture_2d.hpp"
#include "texture/texture_3d.hpp"

/*
 * Factory to produce 2D/3D texture (exploit runtime polymorphism)
 * TODO: bring textures from LevelRenderer & cie here as well
 */
class TexturesFactory {
public:
  TexturesFactory();

  template <typename T>
  T get(const std::string& key) const;

  void free();

private:
  std::unique_ptr<Texture2D> m_health;
  std::unique_ptr<Texture2D> m_crosshair;
  std::unique_ptr<Texture2D> m_wall_diffuse;
  std::unique_ptr<Texture2D> m_wall_normal;
  std::unique_ptr<Texture2D> m_door_diffuse;
  std::unique_ptr<Texture2D> m_door_normal;
  std::unique_ptr<Texture2D> m_floor_diffuse;
  std::unique_ptr<Texture2D> m_floor_normal;
  std::unique_ptr<Texture2D> m_ceiling_diffuse;
  std::unique_ptr<Texture2D> m_ceiling_normal;
  std::unique_ptr<Texture2D> m_window;

  /**
   * using pointers bcos base class Texture is abstract (non-constructible)
   * avoid unique_ptr as values bcos map is init from initializer_list => range ctor => copy items (pair)
   * and copying not allowed with unique_ptr
   */
  std::unordered_map<std::string, Texture*> m_textures;
};

#endif // TEXTURES_FACTORY_HPP
