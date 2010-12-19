/* See LICENSE file for copyright and license details. */
#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <tbb/mutex.h>

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

  Image<uint8_t> get_image(int32_t x, int32_t z, bool* result) const;
  void clear_cache() const;

  // getters
  const MinecraftWorld& world() const { return world_; }
  Settings set() const { return set_; }

  // setters
  void set_world(const MinecraftWorld& _world) { world_ = _world; }
  void set_settings(Settings _set) { set_ = make_valid(_set); }

  // std::list<point3> a_star(int x_start, int z_start,
  //                          int x_end, int z_end);

 private:
  MinecraftWorld world_;
  Settings set_;
  mutable tbb::mutex get_block_mutex_;

  boost::shared_ptr<const std::string> getBlock(std::pair<int, int> block, bool clear = false) const;
  char getValue(int32_t x, int32_t y, int32_t z, int32_t j, int32_t i) const;
  typedef std::map<std::pair<int, int>, boost::shared_ptr<const std::string> > map;
  char getValue(const map& cache,
                   int32_t x, int32_t y, int32_t z, int32_t j, int32_t i) const;

  Color checkReliefDiagonal(const Renderer::map& cache,
                            Color input,
                            int x, int y, int z, int j, int i) const;
  Color checkReliefNormal(const Renderer::map& cache,
                            Color input, int x, int y, int z,
                            int j, int i) const;
  Color calculateMap(const Renderer::map& cache,
                            Color input,
                            int x, int y, int z, int j, int i,
                            int32_t state = 0) const;
  Color calculateShadow(const Renderer::map& cache,
                            Color input,
                            int x, int y, int z, int j, int i,
                            int32_t state = 0) const;
  Color calculateRelief(const Renderer::map& cache,
                            Color input, int x, int y, int z,
                            int j, int i) const;

  Color blockid_to_color(int value, int x, int z, int j, int i,
                         bool oblique = false) const;


  void projectCoords(int32_t& x, int32_t& y, int32_t& z,
                     int32_t xx, int32_t zz, int32_t& state) const;
  void goOneStepIntoScene(int32_t& x, int32_t& y, int32_t& z,
                          int32_t& state) const;
  void changeBlockParts(int32_t& blockid, int state) const;
};

#endif  // _RENDERER_H_
