#ifndef _NBT_H
#define _NBT_H

#include <stdint.h>
#include <list>
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>

#include "zlib.h"

namespace tag {

struct tag_string;

struct tag {
  virtual ~tag();
  virtual int id() = 0;
};

struct tag_end : public tag {
  int id();
};

struct tag_byte : public tag {
  int id();
  tag_byte();
  tag_byte(gzFile*, bool named);
  tag_string* name;
  int8_t p;
};

struct tag_short : public tag {
  int id();
  tag_short();
  tag_short(gzFile*, bool named);
  tag_string* name;
  int16_t p;
};

struct tag_int : public tag {
  int id();
  tag_int();
  tag_int(gzFile*, bool named);
  tag_string* name;
  int32_t p;
};

struct tag_long : public tag {
  int id();
  tag_long();
  tag_long(gzFile*, bool named);
  tag_string* name;
  int64_t p;
};

struct tag_float : public tag {
  int id();
  tag_float();
  tag_float(gzFile*, bool named);
  tag_string* name;
  float p;
};

struct tag_double : public tag {
  int id();
  tag_double();
  tag_double(gzFile*, bool named);
  tag_string* name;
  double p;
};

struct tag_byte_array : public tag {
  int id();
  tag_byte_array();
  tag_byte_array(gzFile*, bool named);
  tag_string* name;
  tag_int length;
  std::string p;
};

struct tag_string : public tag {
  int id();
  tag_string();
  tag_string(gzFile*, bool named);
  tag_string* name;
  tag_short length;
  std::string p;
};

struct tag_list : public tag {
  int id();
  tag_list();
  tag_list(gzFile*, bool named);
  tag_string* name;
  tag_byte tagid;
  tag_int length;
  std::list<tag*> tags;
};

struct tag_compound : public tag {
  int id();
  tag_compound();
  tag_compound(gzFile* file, bool named);
  tag_string* name;
  std::list<tag*> tags;
};

}

class nbt {
 public:
  nbt(const std::string&);

  std::string string();
 private:
  tag::tag_compound global;
};

#endif  // _NBT_H
