#include "./tag.h"

#include <boost/assign.hpp>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace tag {

static int indent = 0;

template <typename T>
T endian_swap(T d) {
  T a = d;
  unsigned char* dst = reinterpret_cast<unsigned char*>(&a);
  unsigned char* src = reinterpret_cast<unsigned char*>(&d);
  for (unsigned int i = 0; i < sizeof(T); ++i) {
    dst[i] = src[sizeof(T)-i-1];
  }
  return a;
}

std::map<int8_t, std::string> tagid_string = boost::assign::map_list_of
  (0, "TAG_End")
  (1, "TAG_Byte")
  (2, "TAG_Short")
  (3, "TAG_Int")
  (4, "TAG_Long")
  (5, "TAG_Float")
  (6, "TAG_Double")
  (7, "TAG_Byte_Array")
  (8, "TAG_String")
  (9, "TAG_List")
  (10, "TAG_Compound")
;

const std::tr1::shared_ptr<tag> tag::sub(const std::string& subname) const {
  return reinterpret_cast<const tag_<compound>*>(this)->pay().sub(subname);
}

template <> int tag_<int8_t>::id() { return 1; }
template <> int tag_<int16_t>::id() { return 2; }
template <> int tag_<int32_t>::id() { return 3; }
template <> int tag_<int64_t>::id() { return 4; }
template <> int tag_<float>::id() { return 5; }
template <> int tag_<double>::id() { return 6; }
template <> int tag_<byte_array>::id() { return 7; }
template <> int tag_<string>::id() { return 8; }
template <> int tag_<list>::id() { return 9; }
template <> int tag_<compound>::id() { return 10; }

template <> tag_<byte_array>::tag_(gzFile* file, bool named)
          : tag(file, named), p(file) {}
template <> tag_<string>::tag_(gzFile* file, bool named)
          : tag(file, named), p(file) {}
template <> tag_<list>::tag_(gzFile* file, bool named)
          : tag(file, named), p(file) {}
template <> tag_<compound>::tag_(gzFile* file, bool named)
          : tag(file, named), p(file) {}

template <typename T>
inline void dont_print_char(std::stringstream& ss, const T& p) {
  ss << p;
}

template <>
inline void dont_print_char<int8_t>(std::stringstream& ss, const int8_t& p) {
  ss << static_cast<int>(p);
}

template <typename T>
std::string tag_<T>::str() {
  std::stringstream ss;
  ss.precision(12);
  ss << std::string(indent, ' ') << tagid_string[id()];
  if (name.get()) {
    ss << "(\"" << name->p.p << "\")";
  }
  ss << ": ";
  dont_print_char<T>(ss, p);
  ss << "\n";
  return ss.str();
}

template <typename T>
tag_<T>::tag_() : tag(), p() {}
template <typename T>
tag_<T>::tag_(gzFile* file, bool named)
          : tag(file, named), p() {
  if (gzread(*file, reinterpret_cast<void*>(&p), sizeof(p)) == -1) {
    std::cerr << "file read error! " << filename << std::endl;
    exit(1);
  }
  if (id() != 0) {
    p = endian_swap<T>(p);
  }
}
template <typename T>
const T& tag_<T>::pay() const { return p; }

tag::tag() : name() {}
tag::~tag() {}
tag::tag(gzFile* file, bool named)
          : name(named ? new tag_<string>(file, false) : NULL) {}


string::string(gzFile* file) : length(file, false), p() {
  char* bufferstring = new char[length.p];
  if (gzread(*file, bufferstring, length.p) == -1) {
    std::cerr << "file read error! " << filename << std::endl;
    exit(1);
  }
  p = std::string(bufferstring, length.p);
  delete[] bufferstring;
}
std::ostream& operator <<(std::ostream& os, const string& obj) {
  os << obj.p;
  return os;
}
byte_array::byte_array(gzFile* file) : length(file, false), p() {
  char* bufferstring = new char[length.p];
  if (gzread(*file, bufferstring, length.p) == -1) {
    std::cerr << "file read error! " << filename << std::endl;
    exit(1);
  }
  p = std::string(bufferstring, length.p);
  delete[] bufferstring;
}
std::ostream& operator <<(std::ostream& os, const byte_array& obj) {
  os << "[" << obj.length.p << " bytes]";
  return os;
}


list::list(gzFile* file) : tagid(file, false), length(file, false),
                           tags(length.p) {
  for (int i = 0; i < length.p; ++i) {
    push_in_tags(&tags, file, tagid.p, false, i);
  }
}
compound::compound(gzFile* file) : tags() {
  int buffer;
  while ((buffer = gzgetc(*file)) != 0) {
    push_in_tags(&tags, file, buffer, true);
  }
}

template<typename T, typename I> void output_stuff(std::ostream& os,
                                                   const T& obj) {
  os << std::string(indent, ' ') << "{\n";

  I i = obj.tags.begin();
  indent += 2;
  for (; i != obj.tags.end(); ++i)
    os << (*i)->str();
  indent -= 2;

  os << std::string(indent, ' ') << "}";
}
std::ostream& operator <<(std::ostream& os, const list& obj) {
  os << obj.tags.size() << " entries of type "
     << tagid_string[obj.tagid.p] << "\n";
  output_stuff<list, std::vector<std::tr1::shared_ptr<tag> >::const_iterator>
              (os, obj);
  return os;
}
std::ostream& operator <<(std::ostream& os, const compound& obj) {
  os << obj.tags.size() << " entries\n";
  os << std::string(indent, ' ') << "{\n";
  output_stuff<compound, std::list<std::tr1::shared_ptr<tag> >::const_iterator>
              (os, obj);
  return os;
}

const std::tr1::shared_ptr<tag> compound::sub(const std::string& name) const {
  std::list<std::tr1::shared_ptr<tag> >::const_iterator it = tags.begin();
  while (it != tags.end()) {
    if ((*it)->name->p.p.compare(name) == 0) {
      return *it;
    }
    ++it;
  }
  return std::tr1::shared_ptr<tag>();
}

void push_in_tags(std::vector<std::tr1::shared_ptr<tag> >* tags, gzFile* file,
                  int switcher, bool with_string, int i) {
  typedef std::tr1::shared_ptr<tag> tp;
  switch (switcher) {
    case -1:
      std::cerr << "file read error! " << filename << std::endl;
      exit(1);
      break;
    case 1: (*tags)[i] = tp(new tag_<int8_t>(file, with_string)); break;
    case 2: (*tags)[i] = tp(new tag_<int16_t>(file, with_string)); break;
    case 3: (*tags)[i] = tp(new tag_<int32_t>(file, with_string)); break;
    case 4: (*tags)[i] = tp(new tag_<int64_t>(file, with_string)); break;
    case 5: (*tags)[i] = tp(new tag_<float>(file, with_string)); break;
    case 6: (*tags)[i] = tp(new tag_<double>(file, with_string)); break;
    case 7: (*tags)[i] = tp(new tag_<byte_array>(file, with_string)); break;
    case 8: (*tags)[i] = tp(new tag_<string>(file, with_string)); break;
    case 9: (*tags)[i] = tp(new tag_<list>(file, with_string)); break;
    case 10: (*tags)[i] = tp(new tag_<compound>(file, with_string)); break;
    default:
      std::cerr << "wrong file format: " << switcher << filename << std::endl;
      exit(1);
      break;
  }
}

void push_in_tags(std::list<std::tr1::shared_ptr<tag> >* tags, gzFile* file,
                  int switcher, bool with_string) {
  typedef std::tr1::shared_ptr<tag> tp;
  switch (switcher) {
    case -1:
      std::cerr << "file read error! " << filename << std::endl;
      exit(1);
      break;
    case 1: tags->push_back(tp(new tag_<int8_t>(file, with_string))); break;
    case 2: tags->push_back(tp(new tag_<int16_t>(file, with_string))); break;
    case 3: tags->push_back(tp(new tag_<int32_t>(file, with_string))); break;
    case 4: tags->push_back(tp(new tag_<int64_t>(file, with_string))); break;
    case 5: tags->push_back(tp(new tag_<float>(file, with_string))); break;
    case 6: tags->push_back(tp(new tag_<double>(file, with_string))); break;
    case 7: tags->push_back(tp(new tag_<byte_array>(file, with_string))); break;
    case 8: tags->push_back(tp(new tag_<string>(file, with_string))); break;
    case 9: tags->push_back(tp(new tag_<list>(file, with_string))); break;
    case 10: tags->push_back(tp(new tag_<compound>(file, with_string))); break;
    default:
      std::cerr << "wrong file format: " << switcher << filename << std::endl;
      exit(1);
      break;
  }
}
}
