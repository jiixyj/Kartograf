#ifndef SRC_NBT_TAG_H_
#define SRC_NBT_TAG_H_

#include <zlib.h>

#include <tr1/memory>
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <vector>

namespace tag {

static std::string filename;

struct string;
template <typename T>
struct tag_;

#define ERROREXIT std::cerr << "Wrong pay_<>() function called!" << std::endl; \
                  exit(1);
struct tag {
  tag();
  tag(gzFile* file, bool named);
  virtual ~tag();
  virtual int id() = 0;
  virtual std::string str() = 0;
  template <class T> const T& pay_() const {
    const tag_<T>* payload = reinterpret_cast<const tag_<T>*>(this);
    return payload->p;
  }
  const std::tr1::shared_ptr<tag> sub(const std::string&) const;
  const std::tr1::shared_ptr<tag_<string> > name;
};

template <typename T>
struct tag_ : public tag {
  int id();
  std::string str();
  tag_();
  tag_(gzFile*, bool named);
  T p;
  const T& pay() const;
};

struct byte_array {
  explicit byte_array(gzFile* file);
  tag_<int32_t> length;
  std::string p;
  friend std::ostream& operator <<(std::ostream& os, const byte_array& obj);
};

struct string {
  explicit string(gzFile* file);
  tag_<int16_t> length;
  std::string p;
  friend std::ostream& operator <<(std::ostream& os, const string& obj);
};

struct list {
  explicit list(gzFile* file);
  tag_<int8_t> tagid;
  tag_<int32_t> length;
  std::vector<std::tr1::shared_ptr<tag> > tags;
  friend std::ostream& operator <<(std::ostream& os, const list& obj);
};

struct compound {
  explicit compound(gzFile* file);
  std::list<std::tr1::shared_ptr<tag> > tags;
  const std::tr1::shared_ptr<tag> sub(const std::string& subname) const;
  friend std::ostream& operator <<(std::ostream& os, const compound& obj);
};

std::ostream& operator <<(std::ostream& os, const byte_array& obj);
std::ostream& operator <<(std::ostream& os, const string& obj);
std::ostream& operator <<(std::ostream& os, const list& obj);
std::ostream& operator <<(std::ostream& os, const compound& obj);

void push_in_tags(std::vector<std::tr1::shared_ptr<tag> >* tags, gzFile* file,
                  int switcher, bool with_string, int index);
void push_in_tags(std::list<std::tr1::shared_ptr<tag> >* tags, gzFile* file,
                  int switcher, bool with_string);
}

#endif  // SRC_NBT_TAG_H_
