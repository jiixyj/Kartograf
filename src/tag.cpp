/* See LICENSE file for copyright and license details. */
#include "./tag.h"

#include <map>
#include <sstream>

namespace tag {

static size_t indent = 0;

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

static std::map<int, std::string> make_tagid_string() {
  std::map<int, std::string> ret;
  ret.insert(std::map<int, std::string>::value_type(0, "TAG_End"));
  ret.insert(std::map<int, std::string>::value_type(1, "TAG_Byte"));
  ret.insert(std::map<int, std::string>::value_type(2, "TAG_Short"));
  ret.insert(std::map<int, std::string>::value_type(3, "TAG_Int"));
  ret.insert(std::map<int, std::string>::value_type(4, "TAG_Long"));
  ret.insert(std::map<int, std::string>::value_type(5, "TAG_Float"));
  ret.insert(std::map<int, std::string>::value_type(6, "TAG_Double"));
  ret.insert(std::map<int, std::string>::value_type(7, "TAG_Byte_Array"));
  ret.insert(std::map<int, std::string>::value_type(8, "TAG_String"));
  ret.insert(std::map<int, std::string>::value_type(9, "TAG_List"));
  ret.insert(std::map<int, std::string>::value_type(10, "TAG_Compound"));
  return ret;
}

std::map<int, std::string> tagid_string = make_tagid_string();

const tag* tag::sub(const std::string& subname) const {
  return reinterpret_cast<const tag_<compound>*>(this)->pay().sub(subname);
}

template <> int tag_<int8_t>::id() const { return 1; }
template <> int tag_<int16_t>::id() const { return 2; }
template <> int tag_<int32_t>::id() const { return 3; }
template <> int tag_<int64_t>::id() const { return 4; }
template <> int tag_<float>::id() const { return 5; }
template <> int tag_<double>::id() const { return 6; }
template <> int tag_<byte_array>::id() const { return 7; }
template <> int tag_<string>::id() const { return 8; }
template <> int tag_<list>::id() const { return 9; }
template <> int tag_<compound>::id() const { return 10; }

// Not specified, but useful
template <> int tag_<uint16_t>::id() const { return 2; }
template <> int tag_<uint32_t>::id() const { return 3; }

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
std::string tag_<T>::str() const {
  std::stringstream ss;
  ss.precision(12);
  ss << std::string(indent, ' ') << tagid_string[id()];
  if (name) {
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
    throw std::runtime_error("file read error! " + filename);
  }
  if (id() != 0) {
    p = endian_swap<T>(p);
  }
}
template <typename T>
const T& tag_<T>::pay() const { return p; }
template <>
void tag_<string>::write_to_file(gzFile* fileout) const {
  if (name) {
    char type = static_cast<char>(id());
    gzwrite(*fileout, &type, 1);
    name->write_to_file(fileout);
  }
  p.length.write_to_file(fileout);
  gzwrite(*fileout, p.p.c_str(), static_cast<unsigned int>(p.p.size()));
}
template <typename T>
void tag_<T>::write_to_file(gzFile* fileout) const {
  if (name) {
    char type = static_cast<char>(id());
    gzwrite(*fileout, &type, 1);
    name->write_to_file(fileout);
  }
  T out = endian_swap<T>(p);
  gzwrite(*fileout, reinterpret_cast<void*>(&out), sizeof(out));
}
template <>
void tag_<byte_array>::write_to_file(gzFile* fileout) const {
  if (name) {
    char type = static_cast<char>(id());
    gzwrite(*fileout, &type, 1);
    name->write_to_file(fileout);
  }
  p.length.write_to_file(fileout);
  gzwrite(*fileout, p.p.c_str(), static_cast<unsigned int>(p.p.size()));
}
template <>
void tag_<list>::write_to_file(gzFile* fileout) const {
  if (name) {
    char type = static_cast<char>(id());
    gzwrite(*fileout, &type, 1);
    name->write_to_file(fileout);
  }
  p.tagid.write_to_file(fileout);
  p.length.write_to_file(fileout);
  for (size_t i = 0; i < p.tags.size(); ++i) {
    p.tags[i]->write_to_file(fileout);
  }
}
template <>
void tag_<compound>::write_to_file(gzFile* fileout) const {
  if (name) {
    char type = static_cast<char>(id());
    gzwrite(*fileout, &type, 1);
    name->write_to_file(fileout);
  }
  for (std::list<tag*>::const_iterator it = p.tags.begin();
       it != p.tags.end(); ++it) {
    (*it)->write_to_file(fileout);
  }
  gzwrite(*fileout, "", 1);
}

tag::tag() : name() {}
tag::~tag() {
  delete name;
}
tag::tag(gzFile* file, bool named)
          : name(named ? new tag_<string>(file, false) : NULL) {}
void tag::write_to_file(const std::string& _filename) const {
  gzFile fileout = gzopen(_filename.c_str(), "wb");
  reinterpret_cast<const tag_<compound>*>(this)->write_to_file(&fileout);
  gzclose(fileout);
}

string::string(gzFile* file) : length(file, false), p() {
  char* bufferstring = new char[length.p];
  if (gzread(*file, bufferstring, length.p) == -1) {
    throw std::runtime_error("file read error! " + filename);
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
    throw std::runtime_error("file read error! " + filename);
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
  for (size_t i = 0; i < length.p; ++i) {
    push_in_tags(&tags, file, tagid.p, false, i);
  }
}

list::~list() {
  while(!tags.empty()) delete tags.back(), tags.pop_back();
}

compound::compound(gzFile* file) : tags() {
  int buffer;
  while ((buffer = gzgetc(*file)) != 0) {
    push_in_tags(&tags, file, buffer, true);
  }
}
static bool delete_all(tag* el) { delete el; return true; }
compound::~compound() {
  tags.remove_if(delete_all);
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
  output_stuff<list, std::vector<tag*>::const_iterator>
              (os, obj);
  return os;
}
std::ostream& operator <<(std::ostream& os, const compound& obj) {
  os << obj.tags.size() << " entries\n";
  output_stuff<compound, std::list<tag*>::const_iterator>
              (os, obj);
  return os;
}

const tag* compound::sub(const std::string& name) const {
  std::list<tag*>::const_iterator it = tags.begin();
  while (it != tags.end()) {
    if ((*it)->name->p.p.compare(name) == 0) {
      return *it;
    }
    ++it;
  }
  return NULL;
}

void push_in_tags(std::vector<tag*>* tags, gzFile* file,
                  int switcher, bool with_string, size_t i) {
  switch (switcher) {
    case -1:
      throw std::runtime_error("file read error! " + filename);
      break;
    case 1: (*tags)[i] = new tag_<int8_t>(file, with_string); break;
    case 2: (*tags)[i] = new tag_<int16_t>(file, with_string); break;
    case 3: (*tags)[i] = new tag_<int32_t>(file, with_string); break;
    case 4: (*tags)[i] = new tag_<int64_t>(file, with_string); break;
    case 5: (*tags)[i] = new tag_<float>(file, with_string); break;
    case 6: (*tags)[i] = new tag_<double>(file, with_string); break;
    case 7: (*tags)[i] = new tag_<byte_array>(file, with_string); break;
    case 8: (*tags)[i] = new tag_<string>(file, with_string); break;
    case 9: (*tags)[i] = new tag_<list>(file, with_string); break;
    case 10: (*tags)[i] = new tag_<compound>(file, with_string); break;
    default:
      throw std::runtime_error("wrong file format: " + switcher + filename);
      break;
  }
}

void push_in_tags(std::list<tag*>* tags, gzFile* file,
                  int switcher, bool with_string) {
  switch (switcher) {
    case -1:
      throw std::runtime_error("file read error! " + filename);
      break;
    case 1: tags->push_back(new tag_<int8_t>(file, with_string)); break;
    case 2: tags->push_back(new tag_<int16_t>(file, with_string)); break;
    case 3: tags->push_back(new tag_<int32_t>(file, with_string)); break;
    case 4: tags->push_back(new tag_<int64_t>(file, with_string)); break;
    case 5: tags->push_back(new tag_<float>(file, with_string)); break;
    case 6: tags->push_back(new tag_<double>(file, with_string)); break;
    case 7: tags->push_back(new tag_<byte_array>(file, with_string)); break;
    case 8: tags->push_back(new tag_<string>(file, with_string)); break;
    case 9: tags->push_back(new tag_<list>(file, with_string)); break;
    case 10: tags->push_back(new tag_<compound>(file, with_string)); break;
    default:
      throw std::runtime_error("wrong file format: " + switcher + filename);
      break;
  }
}

}
