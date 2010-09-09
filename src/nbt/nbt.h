#ifndef SRC_NBT_NBT_H_
#define SRC_NBT_NBT_H_

#include <stdint.h>
#include <zlib.h>

#include <list>
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>

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
  ~tag_byte();
  tag_byte(gzFile*, bool named);
  tag_byte& operator=(const tag_byte& other);
  tag_string* name;
  int8_t p;
 private:
  tag_byte(const tag_byte&);
};

struct tag_short : public tag {
  int id();
  tag_short();
  ~tag_short();
  tag_short(gzFile*, bool named);
  tag_short& operator=(const tag_short& other);
  tag_string* name;
  int16_t p;
 private:
  tag_short(const tag_short&);
};

struct tag_int : public tag {
  int id();
  tag_int();
  ~tag_int();
  tag_int(gzFile*, bool named);
  tag_int& operator=(const tag_int& other);
  tag_string* name;
  int32_t p;
 private:
  tag_int(const tag_int&);
};

struct tag_long : public tag {
  int id();
  tag_long();
  ~tag_long();
  tag_long(gzFile*, bool named);
  tag_string* name;
  int64_t p;
 private:
  tag_long(const tag_long&);
  tag_long& operator=(const tag_long&);
};

struct tag_float : public tag {
  int id();
  tag_float();
  ~tag_float();
  tag_float(gzFile*, bool named);
  tag_string* name;
  float p;
 private:
  tag_float(const tag_float&);
  tag_float& operator=(const tag_float&);
};

struct tag_double : public tag {
  int id();
  tag_double();
  ~tag_double();
  tag_double(gzFile*, bool named);
  tag_string* name;
  double p;
 private:
  tag_double(const tag_double&);
  tag_double& operator=(const tag_double&);
};

struct tag_byte_array : public tag {
  int id();
  tag_byte_array();
  ~tag_byte_array();
  tag_byte_array(gzFile*, bool named);
  tag_string* name;
  tag_int length;
  std::string p;
 private:
  tag_byte_array(const tag_byte_array&);
  tag_byte_array& operator=(const tag_byte_array&);
};

struct tag_string : public tag {
  int id();
  tag_string();
  ~tag_string();
  tag_string(gzFile*, bool named);
  tag_string* name;
  tag_short length;
  std::string p;
 private:
  tag_string(const tag_string&);
  tag_string& operator=(const tag_string&);
};

struct tag_list : public tag {
  int id();
  tag_list();
  ~tag_list();
  tag_list(gzFile*, bool named);
  tag_string* name;
  tag_byte tagid;
  tag_int length;
  std::list<tag*> tags;
 private:
  tag_list(const tag_list&);
  tag_list& operator=(const tag_list&);
};

struct tag_compound : public tag {
  int id();
  tag_compound();
  ~tag_compound();
  tag_compound(gzFile* file, bool named);
  tag_string* name;
  std::list<tag*> tags;
 private:
  tag_compound(const tag_compound&);
  tag_compound& operator=(const tag_compound&);
};

void push_in_tags(std::list<tag*>* tags, gzFile* file,
                  int switcher, bool with_string);
}

class nbt {
 public:
  explicit nbt(const std::string&);
  ~nbt();

  std::string string();
 private:
  tag::tag_compound* global;

  nbt(const nbt&);
  nbt& operator=(const nbt&);
};

#endif  // SRC_NBT_NBT_H_
