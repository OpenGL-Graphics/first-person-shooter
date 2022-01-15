#ifndef GRID_HPP
#define GRID_HPP

#include <vector>

/**
 * Inherited by `Plane` & `Terrain` to set texture UVs & vertexes indices (same for both)
 * `Terrain` cannot be made to inherit from `Plane`, as they have different vertexes positions (z-coords) & normals
 * if that was the case, we'll have positions/normals calculated twice in `Terrain`'s constructor (first by base class, i.e. `Plane`)
 * Grid is the only class in this subfolder that doesn't inherit from `Geometry`
 */
class Grid {
protected:
  /* # of coords by vertex (xyz & normal & texture coords) */
  const unsigned int m_n_coords = 8;

  /* # of vertexes in horizontal & vertical directions */
  unsigned int m_n_vertexes_x;
  unsigned int m_n_vertexes_y;

  /* Protected constructor & no pure virtual methods => abstract class */
  Grid(unsigned int n_vertexes_x, unsigned int n_vertexes_y);

  /* vertexes & indices passed as args (not class members), as they're members of `Geometry` */
  void set_texture_coords(std::vector<float>& vertexes);
  void set_indices(std::vector<unsigned int>& indices);

  void print_indices(std::vector<unsigned int>& indices);
};

#endif // GRID_HPP
