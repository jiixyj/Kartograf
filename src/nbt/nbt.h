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

static std::string filename;

struct tag_string;

struct tag {
  tag();
  tag(gzFile* file, bool named);
  virtual ~tag();
  virtual int id() = 0;
  virtual std::string string(int indent) = 0;
  virtual int8_t pay_byte() const { return -1; }
  virtual int16_t pay_short() const { return -1; }
  virtual int32_t pay_int() const { return -1; }
  virtual int64_t pay_long() const { return -1; }
  virtual float pay_float() const { return -1.0f; }
  virtual double pay_double() const { return -1.0; }
  virtual const std::string& pay_byte_array() const { return tmp2; }
  virtual const std::string& pay_string() const { return tmp2; }
  virtual const std::list<tag*>& pay_list() const { return tmp; }
  virtual const std::list<tag*>& pay_compound() const { return tmp; }
  virtual const tag* sub(const std::string&) const { return NULL; }
  tag_string* name;
 private:
  std::list<tag*> tmp;
  std::string tmp2;

  tag(const tag&);
  tag& operator=(const tag&);
};

struct tag_end : public tag {
  int id();
  std::string string(int indent);
};

struct tag_byte : public tag {
  int id();
  std::string string(int indent);
  tag_byte();
  ~tag_byte();
  tag_byte(gzFile*, bool named);
  tag_byte& operator=(const tag_byte& other);
  int8_t p;
  int8_t pay_byte() const { return p; };
 private:
  tag_byte(const tag_byte&);
};

struct tag_short : public tag {
  int id();
  std::string string(int indent);
  tag_short();
  ~tag_short();
  tag_short(gzFile*, bool named);
  tag_short& operator=(const tag_short& other);
  int16_t p;
  int16_t pay_short() const { return p; };
 private:
  tag_short(const tag_short&);
};

struct tag_int : public tag {
  int id();
  std::string string(int indent);
  tag_int();
  ~tag_int();
  tag_int(gzFile*, bool named);
  tag_int& operator=(const tag_int& other);
  int32_t p;
  int32_t pay_int() const { return p; };
 private:
  tag_int(const tag_int&);
};

struct tag_long : public tag {
  int id();
  std::string string(int indent);
  tag_long();
  ~tag_long();
  tag_long(gzFile*, bool named);
  int64_t p;
  int64_t pay_long() const { return p; };
 private:
  tag_long(const tag_long&);
  tag_long& operator=(const tag_long&);
};

struct tag_float : public tag {
  int id();
  std::string string(int indent);
  tag_float();
  ~tag_float();
  tag_float(gzFile*, bool named);
  float p;
  float pay_float() const { return p; };
 private:
  tag_float(const tag_float&);
  tag_float& operator=(const tag_float&);
};

struct tag_double : public tag {
  int id();
  std::string string(int indent);
  tag_double();
  ~tag_double();
  tag_double(gzFile*, bool named);
  double p;
  double pay_double() const { return p; };
 private:
  tag_double(const tag_double&);
  tag_double& operator=(const tag_double&);
};

struct tag_byte_array : public tag {
  int id();
  std::string string(int indent);
  tag_byte_array();
  ~tag_byte_array();
  tag_byte_array(gzFile*, bool named);
  tag_int length;
  std::string p;
  const std::string& pay_byte_array() const { return p; };
 private:
  tag_byte_array(const tag_byte_array&);
  tag_byte_array& operator=(const tag_byte_array&);
};

struct tag_string : public tag {
  int id();
  std::string string(int indent);
  tag_string();
  ~tag_string();
  tag_string(gzFile*, bool named);
  tag_short length;
  std::string p;
  const std::string& pay_string() const { return p; };
 private:
  tag_string(const tag_string&);
  tag_string& operator=(const tag_string&);
};

struct tag_list : public tag {
  int id();
  std::string string(int indent);
  tag_list();
  ~tag_list();
  tag_list(gzFile*, bool named);
  tag_byte tagid;
  tag_int length;
  std::list<tag*> tags;
  const std::list<tag*>& pay_list() const { return tags; };
 private:
  tag_list(const tag_list&);
  tag_list& operator=(const tag_list&);
};

struct tag_compound : public tag {
  int id();
  std::string string(int indent);
  tag_compound();
  ~tag_compound();
  tag_compound(gzFile* file, bool named);
  std::list<tag*> tags;
  const std::list<tag*>& pay_compound() const { return tags; };
  const tag* sub(const std::string& name) const;
 private:
  tag_compound(const tag_compound&);
  tag_compound& operator=(const tag_compound&);
};

void push_in_tags(std::list<tag*>* tags, gzFile* file,
                  int switcher, bool with_string);
}

class nbt {
 public:
  explicit nbt(int world);
  explicit nbt(const std::string&);
  ~nbt();

  std::string string();

  /* make this private */
  std::list<tag::tag_compound*> global;
 private:

  nbt(const nbt&);
  nbt& operator=(const nbt&);
};

#endif  // SRC_NBT_NBT_H_
