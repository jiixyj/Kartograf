#ifndef SRC_NBT_TAG_H_
#define SRC_NBT_TAG_H_

#include <zlib.h>

#include <stdint.h>
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <stdexcept>
#include <vector>

namespace tag {

static std::string filename;

struct string;
template <typename T>
struct tag_;

struct tag {
  tag();
  tag(gzFile* file, bool named);
  virtual ~tag();
  virtual int id() const = 0;
  virtual std::string str() const = 0;
  template <class T> const T& pay_() const {
    const tag_<T>* payload = dynamic_cast<const tag_<T>*>(this);
    if (!payload) {
      throw std::runtime_error("Wrong pay_<>() function called!");
    }
    return payload->p;
  }
  const tag* sub(const std::string&) const;
  const tag_<string>* name;
 private:
  tag(const tag&);
  tag& operator=(const tag&);
};

template <typename T>
struct tag_ : public tag {
  int id() const;
  std::string str() const;
  tag_();
  tag_(gzFile*, bool named);
  T p;
  const T& pay() const;
};

struct byte_array {
  explicit byte_array(gzFile* file);
  tag_<uint32_t> length;
  std::string p;
  friend std::ostream& operator <<(std::ostream& os, const byte_array& obj);
};

struct string {
  explicit string(gzFile* file);
  tag_<uint16_t> length;
  std::string p;
  friend std::ostream& operator <<(std::ostream& os, const string& obj);
};

struct list {
  explicit list(gzFile* file);
  ~list();
  tag_<int8_t> tagid;
  tag_<uint32_t> length;
  std::vector<tag*> tags;
  friend std::ostream& operator <<(std::ostream& os, const list& obj);
};

struct compound {
  explicit compound(gzFile* file);
  ~compound();
  std::list<tag*> tags;
  const tag* sub(const std::string& subname) const;
  friend std::ostream& operator <<(std::ostream& os, const compound& obj);
};

std::ostream& operator <<(std::ostream& os, const byte_array& obj);
std::ostream& operator <<(std::ostream& os, const string& obj);
std::ostream& operator <<(std::ostream& os, const list& obj);
std::ostream& operator <<(std::ostream& os, const compound& obj);

void push_in_tags(std::vector<tag*>* tags, gzFile* file,
                  int switcher, bool with_string, size_t index);
void push_in_tags(std::list<tag*>* tags, gzFile* file,
                  int switcher, bool with_string);
}

#endif  // SRC_NBT_TAG_H_
