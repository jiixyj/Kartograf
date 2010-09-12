#include <zlib.h>
#include <string>
#include <tr1/memory>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <list>

namespace tag {

static std::string filename;
static int __attribute__ ((__unused__)) indent = 0;

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
    const tag_<T>* payload = dynamic_cast<const tag_<T>*>(this);
    if (!payload) {
      ERROREXIT
    }
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
  byte_array(gzFile*);
  tag_<int32_t> length;
  std::string p;
  friend std::ostream& operator <<(std::ostream& os,const byte_array& obj);
};

struct string {
  string(gzFile*);
  tag_<int16_t> length;
  std::string p;
  friend std::ostream& operator <<(std::ostream& os,const string& obj);
};

struct list {
  list(gzFile*);
  tag_<int8_t> tagid;
  tag_<int32_t> length;
  std::vector<std::tr1::shared_ptr<tag> > tags;
  friend std::ostream& operator <<(std::ostream& os,const list& obj);
};

struct compound {
  compound(gzFile*);
  std::list<std::tr1::shared_ptr<tag> > tags;
  const std::tr1::shared_ptr<tag> sub(const std::string& subname) const;
  friend std::ostream& operator <<(std::ostream& os,const compound& obj);
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
