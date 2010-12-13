/* See LICENSE file for copyright and license details. */
#ifndef SRC_NBT_NBT_H_
#define SRC_NBT_NBT_H_

#include <boost/filesystem.hpp>
#include <tbb/mutex.h>

#include "./tag.h"
#include "./settings.h"
#include "./colors.h"
#include "./image.h"

struct point3 {
  int x;
  int y;
  int z;
};

class nbt {
 public:
  nbt();
  explicit nbt(int world);
  explicit nbt(const std::string&);

  static bool exist_world(int world);

  std::string string() const;
  int32_t xPos_min() const { return xPos_min_; }
  int32_t zPos_min() const { return zPos_min_; }
  int32_t xPos_max() const { return xPos_max_; }
  int32_t zPos_max() const { return zPos_max_; }

  typedef boost::shared_ptr<const tag::tag> tag_ptr;
  bool exists(int32_t x, int32_t z, boost::filesystem::path& path) const;
  tag_ptr tag_at(int32_t x, int32_t z) const;

  Image<uint8_t> getImage(int32_t x, int32_t z, bool* result) const;

  void setSettings(Settings set);
  Settings set() const { return set_; }

  boost::shared_ptr<const std::string> getBlock(std::pair<int, int> block) const;
  char getValue(int32_t x, int32_t y, int32_t z, int32_t j, int32_t i) const;
  typedef std::map<std::pair<int, int>, boost::shared_ptr<const std::string> > map;
  char getValue(const map& cache,
                   int32_t x, int32_t y, int32_t z, int32_t j, int32_t i) const;

  std::list<point3> a_star(int x_start, int z_start,
                           int x_end, int z_end);

  bool bad_world;
  nbt::tag_ptr tag_;
 private:
  mutable tbb::mutex get_block_mutex;

  Color checkReliefDiagonal(const nbt::map& cache, Color input, int x, int y, int z,
                                           int j, int i) const;
  Color checkReliefNormal(const nbt::map& cache, Color input, int x, int y, int z,
                                         int j, int i) const;
  Color calculateMap(const nbt::map& cache, Color input, int x, int y, int z,
                                    int j, int i, int32_t zigzag = 0) const;
  Color calculateShadow(const nbt::map& cache, Color input, int x, int y, int z,
                                       int j, int i, int32_t zigzag = false) const;
  Color calculateRelief(const nbt::map& cache, Color input, int x, int y, int z,
                                       int j, int i) const;

  Color blockid_to_color(int value, int x, int z, int j, int i,
                         bool oblique = false) const;

  int32_t xPos_min_;
  int32_t zPos_min_;
  int32_t xPos_max_;
  int32_t zPos_max_;
  std::set<std::pair<int, int> > valid_coordinates;

  boost::filesystem::path dir_;
  Settings set_;

  bool has_biome_data;
  std::vector<char> foliage_data;
  std::vector<char> grass_data;
  std::map<std::pair<int, int>, std::vector<uint16_t> > biome_indices;

  void construct_world();
  void projectCoords(int32_t& x, int32_t& y, int32_t& z,
                     int32_t xx, int32_t zz, int32_t& state) const;
  void goOneStepIntoScene(int32_t& x, int32_t& y, int32_t& z,
                             int32_t& state) const;
  void changeBlockParts(int32_t& blockid, int state) const;
};

#endif  // SRC_NBT_NBT_H_
