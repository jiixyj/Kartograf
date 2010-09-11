#ifndef SRC_NBT_NBT_H_
#define SRC_NBT_NBT_H_

#include <stdint.h>
#include <zlib.h>

#include <list>
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <tr1/memory>

namespace tag {

static std::string filename;

struct tag_string;

#define ERROREXIT std::cerr << "Wrong pay_*() function called!" << std::endl; \
                  exit(1);
struct tag {
  tag();
  tag(gzFile* file, bool named);
  virtual ~tag();
  virtual int id() = 0;
  virtual std::string string(int indent) = 0;
  virtual int8_t pay_byte() const { ERROREXIT }
  virtual int16_t pay_short() const { ERROREXIT }
  virtual int32_t pay_int() const { ERROREXIT }
  virtual int64_t pay_long() const { ERROREXIT }
  virtual float pay_float() const { ERROREXIT }
  virtual double pay_double() const { ERROREXIT }
  virtual const std::string& pay_byte_array() const { ERROREXIT }
  virtual const std::string& pay_string() const { ERROREXIT }
  virtual const std::list<std::tr1::shared_ptr<tag> >& pay_list() const
  { ERROREXIT }
  virtual const std::list<std::tr1::shared_ptr<tag> >& pay_compound() const
  { ERROREXIT }
  virtual const std::tr1::shared_ptr<tag> sub(const std::string&) const
  { ERROREXIT }
  std::tr1::shared_ptr<tag_string> name;
};

struct tag_byte : public tag {
  int id();
  std::string string(int indent);
  tag_byte();
  tag_byte(gzFile*, bool named);
  int8_t p;
  int8_t pay_byte() const { return p; }
};

struct tag_short : public tag {
  int id();
  std::string string(int indent);
  tag_short();
  tag_short(gzFile*, bool named);
  int16_t p;
  int16_t pay_short() const { return p; }
};

struct tag_int : public tag {
  int id();
  std::string string(int indent);
  tag_int();
  tag_int(gzFile*, bool named);
  int32_t p;
  int32_t pay_int() const { return p; }
};

struct tag_long : public tag {
  int id();
  std::string string(int indent);
  tag_long();
  tag_long(gzFile*, bool named);
  int64_t p;
  int64_t pay_long() const { return p; }
};

struct tag_float : public tag {
  int id();
  std::string string(int indent);
  tag_float();
  tag_float(gzFile*, bool named);
  float p;
  float pay_float() const { return p; }
};

struct tag_double : public tag {
  int id();
  std::string string(int indent);
  tag_double();
  tag_double(gzFile*, bool named);
  double p;
  double pay_double() const { return p; }
};

struct tag_byte_array : public tag {
  int id();
  std::string string(int indent);
  tag_byte_array();
  tag_byte_array(gzFile*, bool named);
  tag_int length;
  std::string p;
  const std::string& pay_byte_array() const { return p; }
};

struct tag_string : public tag {
  int id();
  std::string string(int indent);
  tag_string();
  tag_string(gzFile*, bool named);
  tag_short length;
  std::string p;
  const std::string& pay_string() const { return p; }
};

struct tag_list : public tag {
  int id();
  std::string string(int indent);
  tag_list();
  tag_list(gzFile*, bool named);
  tag_byte tagid;
  tag_int length;
  std::list<std::tr1::shared_ptr<tag> > tags;
  const std::list<std::tr1::shared_ptr<tag> >& pay_list() const { return tags; }
};

struct tag_compound : public tag {
  int id();
  std::string string(int indent);
  tag_compound();
  tag_compound(gzFile* file, bool named);
  std::list<std::tr1::shared_ptr<tag> > tags;
  const std::list<std::tr1::shared_ptr<tag> >& pay_compound() const
  { return tags; }
  const std::tr1::shared_ptr<tag> sub(const std::string& subname) const;
};

void push_in_tags(std::list<std::tr1::shared_ptr<tag> >* tags, gzFile* file,
                  int switcher, bool with_string);
}

class nbt {
 public:
  explicit nbt(int world);
  explicit nbt(const std::string&);

  std::string string();

  /* make this private */
  std::list<std::tr1::shared_ptr<tag::tag_compound> > global;
 private:

  nbt(const nbt&);
  nbt& operator=(const nbt&);
};

#endif  // SRC_NBT_NBT_H_
