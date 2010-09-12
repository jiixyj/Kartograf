#ifndef SRC_NBT_NBT_H_
#define SRC_NBT_NBT_H_

#include <stdint.h>
#include <zlib.h>

#include <string>

#include "./tag.h"

class nbt {
 public:
  explicit nbt(int world);
  explicit nbt(const std::string&);

  std::string string();

  /* make this private */
  std::list<std::tr1::shared_ptr<tag::tag_<tag::compound> > > global;
 private:

  nbt(const nbt&);
  nbt& operator=(const nbt&);
};

#endif  // SRC_NBT_NBT_H_
