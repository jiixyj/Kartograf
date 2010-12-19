/* See LICENSE file for copyright and license details. */
#ifndef SRC_NBT_NBT_H_
#define SRC_NBT_NBT_H_

#include <boost/filesystem.hpp>
#include <tbb/mutex.h>

#include "./tag.h"
#include "./settings.h"
#include "./colors.h"
#include "./image.h"
#include "./minecraft_world.h"

struct point3 {
  int x;
  int y;
  int z;
};

class Renderer {
 public:
  Renderer(const MinecraftWorld&, Settings set);

  const MinecraftWorld& world() const { return world_; }
  void set_world(const MinecraftWorld& _world) { world_ = _world; }

  void clear_cache() const;

  Image<uint8_t> getImage(int32_t x, int32_t z, bool* result) const;

  void setSettings(Settings set);
  Settings set() const { return set_; }

  boost::shared_ptr<const std::string> getBlock(std::pair<int, int> block, bool clear = false) const;
  char getValue(int32_t x, int32_t y, int32_t z, int32_t j, int32_t i) const;
  typedef std::map<std::pair<int, int>, boost::shared_ptr<const std::string> > map;
  char getValue(const map& cache,
                   int32_t x, int32_t y, int32_t z, int32_t j, int32_t i) const;

  std::list<point3> a_star(int x_start, int z_start,
                           int x_end, int z_end);

 private:
  MinecraftWorld world_;

  mutable tbb::mutex get_block_mutex;

  Color checkReliefDiagonal(const Renderer::map& cache, Color input, int x, int y, int z,
                                           int j, int i) const;
  Color checkReliefNormal(const Renderer::map& cache, Color input, int x, int y, int z,
                                         int j, int i) const;
  Color calculateMap(const Renderer::map& cache, Color input, int x, int y, int z,
                                    int j, int i, int32_t zigzag = 0) const;
  Color calculateShadow(const Renderer::map& cache, Color input, int x, int y, int z,
                                       int j, int i, int32_t zigzag = false) const;
  Color calculateRelief(const Renderer::map& cache, Color input, int x, int y, int z,
                                       int j, int i) const;

  Color blockid_to_color(int value, int x, int z, int j, int i,
                         bool oblique = false) const;

  Settings set_;

  void projectCoords(int32_t& x, int32_t& y, int32_t& z,
                     int32_t xx, int32_t zz, int32_t& state) const;
  void goOneStepIntoScene(int32_t& x, int32_t& y, int32_t& z,
                             int32_t& state) const;
  void changeBlockParts(int32_t& blockid, int state) const;
};

#endif  // SRC_NBT_NBT_H_
