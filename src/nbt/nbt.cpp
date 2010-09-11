#include "./nbt.h"

#include <QtCore>

#include <string>
#include <list>
#include <sstream>
#include <boost/assign.hpp>

namespace tag {

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

template<> int tag_<int8_t>::id() { return 1; }
template<> int tag_<int16_t>::id() { return 2; }
template<> int tag_<int32_t>::id() { return 3; }
template<> int tag_<int64_t>::id() { return 4; }
template<> int tag_<float>::id() { return 5; }
template<> int tag_<double>::id() { return 6; }
template<> int tag_<byte_array>::id() { return 7; }
template<> int tag_<string>::id() { return 8; }
template<> int tag_<list>::id() { return 9; }
template<> int tag_<compound>::id() { return 10; }

template <> tag_<byte_array>::tag_(gzFile* file, bool named)
          : tag(file, named), p(file) {}
template <> tag_<string>::tag_(gzFile* file, bool named)
          : tag(file, named), p(file) {}
template <> tag_<list>::tag_(gzFile* file, bool named)
          : tag(file, named), p(file) {}
template <> tag_<compound>::tag_(gzFile* file, bool named)
          : tag(file, named), p(file) {}

template <typename T>
std::string tag_<T>::str() {
  std::stringstream ss;
  ss.precision(12);
  ss << std::string(indent, ' ') << tagid_string[id()];
  if (name.get()) {
    ss << "(\"" << name->p.p << "\")";
  }
  ss << ": " << p << "\n";
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
  os << "[" << obj.length.p << " bytes]" << "\n";
  return os;
}

list::list(gzFile* file) : tagid(file, false), length(file, false), tags() {
  for (int i = 0; i < length.p; ++i) {
    push_in_tags(&tags, file, tagid.p, false);
  }
}
std::ostream& operator <<(std::ostream& os, const list& obj) {
  os << obj.tags.size() << " entries of type "
     << tagid_string[obj.tagid.p] << "\n";
  os << std::string(indent, ' ') << "{\n";

  std::list<std::tr1::shared_ptr<tag> >::const_iterator i = obj.tags.begin();
  indent += 2;
  for (; i != obj.tags.end(); ++i)
    os << (*i)->str();
  indent -= 2;

  os << std::string(indent, ' ') << "}";
  return os;
}

compound::compound(gzFile* file) : tags() {
  int buffer;
  while ((buffer = gzgetc(*file)) != 0) {
    push_in_tags(&tags, file, buffer, true);
  }
}
std::ostream& operator <<(std::ostream& os, const compound& obj) {
  os << obj.tags.size() << " entries\n";
  os << std::string(indent, ' ') << "{\n";

  std::list<std::tr1::shared_ptr<tag> >::const_iterator i = obj.tags.begin();
  indent += 2;
  for (; i != obj.tags.end(); ++i)
    os << (*i)->str();
  indent -= 2;

  os << std::string(indent, ' ') << "}";
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
  std::cerr << "tag not found!" << std::endl;
  ERROREXIT
}

void push_in_tags(std::list<std::tr1::shared_ptr<tag> >* tags, gzFile* file,
                  int switcher, bool with_string) {
  switch (switcher) {
    case -1:
      std::cerr << "file read error! " << filename << std::endl;
      exit(1);
      break;
    case 1:
      tags->push_back(std::tr1::shared_ptr<tag>
                      (new tag_<int8_t>(file, with_string)));
      break;
    case 2:
      tags->push_back(std::tr1::shared_ptr<tag>
                      (new tag_<int16_t>(file, with_string)));
      break;
    case 3:
      tags->push_back(std::tr1::shared_ptr<tag>
                      (new tag_<int32_t>(file, with_string)));
      break;
    case 4:
      tags->push_back(std::tr1::shared_ptr<tag>
                      (new tag_<int64_t>(file, with_string)));
      break;
    case 5:
      tags->push_back(std::tr1::shared_ptr<tag>
                      (new tag_<float>(file, with_string)));
      break;
    case 6:
      tags->push_back(std::tr1::shared_ptr<tag>
                      (new tag_<double>(file, with_string)));
      break;
    case 7:
      tags->push_back(std::tr1::shared_ptr<tag>
                      (new tag_<byte_array>(file, with_string)));
      break;
    case 8:
      tags->push_back(std::tr1::shared_ptr<tag>
                      (new tag_<string>(file, with_string)));
      break;
    case 9:
      tags->push_back(std::tr1::shared_ptr<tag>
                      (new tag_<list>(file, with_string)));
      break;
    case 10:
      tags->push_back(std::tr1::shared_ptr<tag>
                      (new tag_<compound>(file, with_string)));
      break;
    default:
      std::cerr << "wrong file format: " << switcher << filename << std::endl;
      exit(1);
      break;
  }
}
}

nbt::nbt(int world) : global() {
  QDir dir = QDir::home();
  if (!dir.cd(QString(".minecraft/saves/World") + QString::number(world))) {
    qFatal("Minecraft is not installed!");
  }

  QString name = "map" + QString::number(world);
  dir.setFilter(QDir::Files);
  QDirIterator it(dir, QDirIterator::Subdirectories);
  while (it.hasNext()) {
    tag::filename = it.next().toAscii().data();
    size_t first = tag::filename.find(".", tag::filename.find_last_of("/") + 1);
    size_t second = tag::filename.find(".", first + 1);
    long x = strtol(&(tag::filename.c_str()[first + 1]), NULL, 36);
    long z = strtol(&(tag::filename.c_str()[second + 1]), NULL, 36);
    std::cout << tag::filename << "  "<< x << " " << z << std::endl;
    gzFile filein = gzopen(tag::filename.c_str(), "rb");
    if (!filein) {
      std::cerr << "file could not be opened! " << tag::filename << std::endl;
      exit(1);
    }
    int buffer = gzgetc(filein);
    switch (buffer) {
      case -1:
        std::cerr << "file read error! " << tag::filename << std::endl;
        gzclose(filein);
        continue;
      case 10:
        global.push_back(std::tr1::shared_ptr<tag::tag_<tag::compound> >
                                 (new tag::tag_<tag::compound>(&filein, true)));
        break;
      default:
        std::cerr << "wrong file format! " << tag::filename << std::endl;
        gzclose(filein);
        continue;
    }
    gzclose(filein);
  }
}

nbt::nbt(const std::string& filename) : global() {
  tag::filename = filename;
  gzFile filein = gzopen(filename.c_str(), "rb");
  if (!filein) {
    std::cerr << "file could not be opened! " << filename << std::endl;
    exit(1);
  }

  int buffer = gzgetc(filein);
  switch (buffer) {
    case -1:
      std::cerr << "file read error! " << filename << std::endl;
      exit(1);
      break;
    case 10:
      global.push_back(std::tr1::shared_ptr<tag::tag_<tag::compound> >
                                 (new tag::tag_<tag::compound>(&filein, true)));
      break;
    default:
      std::cerr << "wrong file format! " << filename << std::endl;
      exit(1);
      break;
  }
  gzclose(filein);
}

std::string nbt::string() {
  std::stringstream ss;
  std::list<std::tr1::shared_ptr<tag::tag_<tag::compound> > >::iterator i =
                                                                 global.begin();
  for (; i != global.end(); ++i) {
    ss << (*i)->str();
  }
  return ss.str();
}
