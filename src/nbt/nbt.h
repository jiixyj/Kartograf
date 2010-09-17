#ifndef SRC_NBT_NBT_H_
#define SRC_NBT_NBT_H_

#include <stdint.h>

#include <QtGui>
#include <map>
#include <string>
#include <utility>
#include <tbb/concurrent_hash_map.h>

#include "./tag.h"
#include "./settings.h"

class nbt {
 public:
  nbt();
  explicit nbt(int world);
  explicit nbt(const std::string&);

  std::string string() const;
  int32_t xPos_min() const { return xPos_min_; }
  int32_t zPos_min() const { return zPos_min_; }
  int32_t xPos_max() const { return xPos_max_; }
  int32_t zPos_max() const { return zPos_max_; }

  typedef std::tr1::shared_ptr<tag::tag> tag_ptr;
  const tag_ptr tag_at(int32_t x, int32_t z) const;

  void setSettings(Settings set);
  Settings set() const { return set_; }
  typedef tbb::concurrent_hash_map<std::pair<int, int>, std::string> map;
  uint8_t getValue(const map& cache,
                   int32_t x, int32_t y, int32_t z, int32_t j, int32_t i) const;
  QImage getImage(int32_t x, int32_t z, bool* result) const;
  void clearCache() const;

  tag_ptr tag_;
 private:
  QColor checkReliefDiagonal(const nbt::map& cache, QColor input, int x, int y, int z,
                                           int j, int i) const;
  QColor checkReliefNormal(const nbt::map& cache, QColor input, int x, int y, int z,
                                         int j, int i) const;
  QColor calculateMap(const nbt::map& cache, QColor input, int x, int y, int z,
                                    int j, int i, bool zigzag = false) const;
  QColor calculateShadow(const nbt::map& cache, QColor input, int x, int y, int z,
                                       int j, int i) const;
  QColor calculateRelief(const nbt::map& cache, QColor input, int x, int y, int z,
                                       int j, int i) const;
  int32_t xPos_min_;
  int32_t zPos_min_;
  int32_t xPos_max_;
  int32_t zPos_max_;

  QDir dir_;
  Settings set_;

  mutable map blockcache_;

  void construct_world();

  nbt(const nbt&);
  nbt& operator=(const nbt&);
};

#endif  // SRC_NBT_NBT_H_
