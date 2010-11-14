#ifndef SRC_NBT_NBT_H_
#define SRC_NBT_NBT_H_

#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>

#include "./tag.h"
#include "./settings.h"
#include "./colors.h"
#include "./image.h"

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

  void setSettings(Settings set);
  Settings set() const { return set_; }
  typedef std::map<std::pair<int, int>, std::string> map;
  char getValue(const map& cache,
                   int32_t x, int32_t y, int32_t z, int32_t j, int32_t i) const;
  Image<uint8_t> getImage(int32_t x, int32_t z, bool* result) const;
  void clearCache() const;

  nbt::tag_ptr tag_;
 private:
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
  int32_t xPos_min_;
  int32_t zPos_min_;
  int32_t xPos_max_;
  int32_t zPos_max_;

  boost::filesystem::path dir_;
  Settings set_;

  mutable boost::shared_ptr<boost::mutex> cache_mutex_;
  mutable map blockcache_;

  void construct_world();
  void projectCoords(int32_t& x, int32_t& y, int32_t& z,
                     int32_t xx, int32_t zz, int32_t& state) const;
  void goOneStepIntoScene(int32_t& x, int32_t& y, int32_t& z,
                             int32_t& state) const;
  void changeBlockParts(int32_t& blockid, int state) const;
};

#endif  // SRC_NBT_NBT_H_
