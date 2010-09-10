#include "./nbt.h"

#include <QtCore>

#include <string>
#include <list>
#include <sstream>


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

std::string tagid_string(int8_t tagid) {
  switch (tagid) {
    case 0:
      return "TAG_End";
    case 1:
      return "TAG_Byte";
    case 2:
      return "TAG_Short";
    case 3:
      return "TAG_Int";
    case 4:
      return "TAG_Long";
    case 5:
      return "TAG_Float";
    case 6:
      return "TAG_Double";
    case 7:
      return "TAG_Byte_Array";
    case 8:
      return "TAG_String";
    case 9:
      return "TAG_List";
    case 10:
      return "TAG_Compound";
    default:
      return "";
  }
}

int tag_end::id() { return 0; }
int tag_byte::id() { return 1; }
int tag_short::id() { return 2; }
int tag_int::id() { return 3; }
int tag_long::id() { return 4; }
int tag_float::id() { return 5; }
int tag_double::id() { return 6; }
int tag_byte_array::id() { return 7; }
int tag_string::id() { return 8; }
int tag_list::id() { return 9; }
int tag_compound::id() { return 10; }

std::string tag_end::string(int) {
  return "";
}
std::string tag_byte::string(int indent) {
  std::stringstream ss;
  ss << std::string(indent, ' ') << tagid_string(id());
  if (name) {
    ss << "(\"" << name->p << "\")";
  }
  ss << ": " << static_cast<int>(p) << "\n";
  return ss.str();
}
std::string tag_short::string(int indent) {
  std::stringstream ss;
  ss << std::string(indent, ' ') << tagid_string(id());
  if (name) {
    ss << "(\"" << name->p << "\")";
  }
  ss << ": " << p << "\n";
  return ss.str();
}
std::string tag_int::string(int indent) {
  std::stringstream ss;
  ss << std::string(indent, ' ') << tagid_string(id());
  if (name) {
    ss << "(\"" << name->p << "\")";
  }
  ss << ": " << p << "\n";
  return ss.str();
}
std::string tag_long::string(int indent) {
  std::stringstream ss;
  ss << std::string(indent, ' ') << tagid_string(id());
  if (name) {
    ss << "(\"" << name->p << "\")";
  }
  ss << ": " << p << "\n";
  return ss.str();
}
std::string tag_float::string(int indent) {
  std::stringstream ss;
  ss.precision(12);
  ss << std::string(indent, ' ') << tagid_string(id());
  if (name) {
    ss << "(\"" << name->p << "\")";
  }
  ss << ": " << p << "\n";
  return ss.str();
}
std::string tag_double::string(int indent) {
  std::stringstream ss;
  ss.precision(12);
  ss << std::string(indent, ' ') << tagid_string(id());
  if (name) {
    ss << "(\"" << name->p << "\")";
  }
  ss << ": " << p << "\n";
  return ss.str();
}
std::string tag_byte_array::string(int indent) {
  std::stringstream ss;
  ss << std::string(indent, ' ') << tagid_string(id());
  if (name) {
    ss << "(\"" << name->p << "\")";
  }
  ss << ": [" << length.p << " bytes]" << "\n";
  return ss.str();
}
std::string tag_string::string(int indent) {
  std::stringstream ss;
  ss << std::string(indent, ' ') << tagid_string(id());
  if (name) {
    ss << "(\"" << name->p << "\")";
  }
  ss << ": " << p << "\n";
  return ss.str();
}
std::string tag_list::string(int indent) {
  std::stringstream ss;
  ss << std::string(indent, ' ') << tagid_string(id());
  if (name) {
    ss << "(\"" << name->p << "\")";
  }
  ss << ": " << tags.size() << " entries of type "
             << tagid_string(tagid.p) << "\n";
  ss << std::string(indent, ' ') << "{\n";

  for (std::list<tag*>::iterator i = tags.begin(); i != tags.end(); ++i)
    ss << (*i)->string(indent + 2);

  ss << std::string(indent, ' ') << "}\n";
  return ss.str();
}
std::string tag_compound::string(int indent) {
  std::stringstream ss;
  ss << std::string(indent, ' ') << tagid_string(id());
  if (name) {
    ss << "(\"" << name->p << "\")";
  }
  ss << ": " << tags.size() << " entries\n";
  ss << std::string(indent, ' ') << "{\n";

  for (std::list<tag*>::iterator i = tags.begin(); i != tags.end(); ++i)
    ss << (*i)->string(indent + 2);

  ss << std::string(indent, ' ') << "}\n";
  return ss.str();
}

tag_byte& tag_byte::operator=(const tag_byte& other) {
  if (other.name != NULL) exit(1);
  name = other.name;
  p = other.p;
  return *this;
}

tag_short& tag_short::operator=(const tag_short& other) {
  if (other.name != NULL) exit(1);
  name = other.name;
  p = other.p;
  return *this;
}

tag_int& tag_int::operator=(const tag_int& other) {
  if (other.name != NULL) exit(1);
  name = other.name;
  p = other.p;
  return *this;
}

tag::tag() : name(NULL), tmp(), tmp2() {}
tag::tag(gzFile* file, bool named)
          : name(named ? new tag_string(file, false) : NULL), tmp(), tmp2() {}
tag::~tag() {
  delete name;
  name = NULL;
}

tag_byte::tag_byte() : tag(), p() {}
tag_byte::~tag_byte() {
}
tag_byte::tag_byte(gzFile* file, bool named)
          : tag(file, named), p() {
  if (gzread(*file, reinterpret_cast<void*>(&p), sizeof(p)) == -1) {
    std::cerr << "file read error! " << filename << std::endl;
    exit(1);
  }
}

tag_short::tag_short() : tag(), p() {}
tag_short::~tag_short() {
}
tag_short::tag_short(gzFile* file, bool named)
          : tag(file, named), p() {
  if (gzread(*file, reinterpret_cast<void*>(&p), sizeof(p)) == -1) {
    std::cerr << "file read error! " << filename << std::endl;
    exit(1);
  }
  p = endian_swap<int16_t>(p);
}

tag_int::tag_int() : tag(), p() {}
tag_int::~tag_int() {
}
tag_int::tag_int(gzFile* file, bool named)
          : tag(file, named), p() {
  if (gzread(*file, reinterpret_cast<void*>(&p), sizeof(p)) == -1) {
    std::cerr << "file read error! " << filename << std::endl;
    exit(1);
  }
  p = endian_swap<int32_t>(p);
}

tag_long::tag_long() : tag(), p() {}
tag_long::~tag_long() {
}
tag_long::tag_long(gzFile* file, bool named)
          : tag(file, named), p() {
  if (gzread(*file, reinterpret_cast<void*>(&p), sizeof(p)) == -1) {
    std::cerr << "file read error! " << filename << std::endl;
    exit(1);
  }
  p = endian_swap<int64_t>(p);
}

tag_float::tag_float() : tag(), p() {}
tag_float::~tag_float() {
}
tag_float::tag_float(gzFile* file, bool named)
          : tag(file, named), p() {
  if (gzread(*file, reinterpret_cast<void*>(&p), sizeof(p)) == -1) {
    std::cerr << "file read error! " << filename << std::endl;
    exit(1);
  }
  p = endian_swap<float>(p);
}

tag_double::tag_double() : tag(), p() {}
tag_double::~tag_double() {
}
tag_double::tag_double(gzFile* file, bool named)
          : tag(file, named), p() {
  if (gzread(*file, reinterpret_cast<void*>(&p), sizeof(p)) == -1) {
    std::cerr << "file read error! " << filename << std::endl;
    exit(1);
  }
  p = endian_swap<double>(p);
}

tag_byte_array::tag_byte_array() : tag(), length(), p() {}
tag_byte_array::~tag_byte_array() {
}
tag_byte_array::tag_byte_array(gzFile* file, bool named)
          : tag(file, named), length(), p() {
  length = tag_int(file, false);
  char* bufferstring = new char[length.p];
  if (gzread(*file, bufferstring, length.p) == -1) {
    std::cerr << "file read error! " << filename << std::endl;
    exit(1);
  }
  p = std::string(bufferstring, length.p);
  delete[] bufferstring;
}

tag_string::tag_string() : tag(), length(), p() {}
tag_string::~tag_string() {
}
tag_string::tag_string(gzFile* file, bool named)
          : tag(file, named), length(), p() {
  length = tag_short(file, false);
  char* bufferstring = new char[length.p];
  if (gzread(*file, bufferstring, length.p) == -1) {
    std::cerr << "file read error! " << filename << std::endl;
    exit(1);
  }
  p = std::string(bufferstring, length.p);
  delete[] bufferstring;
}

tag_list::tag_list() : tag(), tagid(), length(), tags() {}
tag_list::~tag_list() {
  for (std::list<tag*>::iterator i = tags.begin(); i != tags.end();) {
    delete *i++;
  }
}
tag_list::tag_list(gzFile* file, bool named)
          : tag(file, named),
            tagid(), length(), tags() {
  tagid = tag_byte(file, false);
  length = tag_int(file, false);
  for (int i = 0; i < length.p; ++i) {
    push_in_tags(&tags, file, tagid.p, false);
  }
}

tag_compound::tag_compound() : tag(), tags() {}
tag_compound::~tag_compound() {
  for (std::list<tag*>::iterator i = tags.begin(); i != tags.end();) {
    delete *i++;
  }
}
tag_compound::tag_compound(gzFile* file, bool named)
          : tag(file, named), tags() {
  int buffer;
  while ((buffer = gzgetc(*file)) != 0) {
    push_in_tags(&tags, file, buffer, true);
  }
}

const tag* tag_compound::sub(const std::string& subname) const {
  std::list<tag*>::const_iterator it = tags.begin();
  while (it != tags.end()) {
    if ((*it)->name->p.compare(subname) == 0) {
      return *it;
    }
    ++it;
  }
  std::cerr << "tag not found!" << std::endl;
  return NULL;
}

void push_in_tags(std::list<tag*>* tags, gzFile* file,
                  int switcher, bool with_string) {
  switch (switcher) {
    case -1:
      std::cerr << "file read error! " << filename << std::endl;
      exit(1);
      break;
    case 1:
      tags->push_back(new tag_byte(file, with_string));
      break;
    case 2:
      tags->push_back(new tag_short(file, with_string));
      break;
    case 3:
      tags->push_back(new tag_int(file, with_string));
      break;
    case 4:
      tags->push_back(new tag_long(file, with_string));
      break;
    case 5:
      tags->push_back(new tag_float(file, with_string));
      break;
    case 6:
      tags->push_back(new tag_double(file, with_string));
      break;
    case 7:
      tags->push_back(new tag_byte_array(file, with_string));
      break;
    case 8:
      tags->push_back(new tag_string(file, with_string));
      break;
    case 9:
      tags->push_back(new tag_list(file, with_string));
      break;
    case 10:
      tags->push_back(new tag_compound(file, with_string));
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
        global.push_back(new tag::tag_compound(&filein, true));
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
      global.push_back(new tag::tag_compound(&filein, true));
      break;
    default:
      std::cerr << "wrong file format! " << filename << std::endl;
      exit(1);
      break;
  }
  gzclose(filein);
}

nbt::~nbt() {
  for (std::list<tag::tag_compound*>::iterator i = global.begin();
                                                           i != global.end();) {
    delete *i++;
  }
}

std::string nbt::string() {
  std::stringstream ss;
  for (std::list<tag::tag_compound*>::iterator i = global.begin();
                                                       i != global.end(); ++i) {
    ss << (*i)->string(0);
  }
  return ss.str();
}

