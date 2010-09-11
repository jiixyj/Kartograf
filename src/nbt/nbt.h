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

struct string;
template <typename T>
struct tag_;

#define ERROREXIT std::cerr << "Wrong pay_*() function called!" << std::endl; \
                  exit(1);
struct tag {
  tag();
  tag(gzFile* file, bool named);
  virtual ~tag();
  virtual int id() = 0;
  virtual std::string str(int indent) = 0;
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
  std::tr1::shared_ptr<tag_<string> > name;
};

struct FalseType { enum { value = false }; };
struct TrueType { enum { value = true }; };
template <typename T1, typename T2> struct IsSame { typedef FalseType Result; };
template <typename T> struct IsSame<T,T> { typedef TrueType Result; };

template <typename T>
struct tag_ : public tag {
  int id();
  std::string str(int indent);
  tag_();
  tag_(gzFile*, bool named);
  T p;
  const T& pay() const;
};

struct byte_array {
  byte_array(gzFile*, bool named);
  tag_<int32_t> length;
  std::string p;
};

struct string {
  string(gzFile*, bool named);
  tag_<int16_t> length;
  std::string p;
  friend std::ostream& operator <<(std::ostream& os,const string& obj);
};
std::ostream& operator <<(std::ostream& os,const string& obj);

struct list {
  list(gzFile*, bool named);
  tag_<int8_t> tagid;
  tag_<int32_t> length;
  std::list<std::tr1::shared_ptr<tag> > tags;
};

struct compound {
  std::list<std::tr1::shared_ptr<tag> > tags;
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
  std::list<std::tr1::shared_ptr<tag::tag_<tag::compound> > > global;
 private:

  nbt(const nbt&);
  nbt& operator=(const nbt&);
};

#endif  // SRC_NBT_NBT_H_
