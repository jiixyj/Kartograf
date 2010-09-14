#ifndef SRC_NBT_NBT_H_
#define SRC_NBT_NBT_H_

#include <stdint.h>
#include <zlib.h>

#include <string>
#include <QtGui>

#include "./tag.h"
#include "./settings.h"

class nbt {
 public:
  nbt();
  explicit nbt(int world);
  explicit nbt(const std::string&);

  std::string string();
  int32_t xPos_min() const { return xPos_min_; }
  int32_t zPos_min() const { return zPos_min_; }
  int32_t xPos_max() const { return xPos_max_; }
  int32_t zPos_max() const { return zPos_max_; }

  typedef std::tr1::shared_ptr<tag::tag> tag_ptr;
  const tag_ptr tag_at(int32_t x, int32_t z) const;

  void setSettings(Settings set);
  QImage getImage(int32_t x, int32_t z) const;

  tag_ptr tag_;
 private:
  int32_t xPos_min_;
  int32_t zPos_min_;
  int32_t xPos_max_;
  int32_t zPos_max_;

  QDir dir_;
  Settings set_;

  void construct_world();
};

#endif  // SRC_NBT_NBT_H_
