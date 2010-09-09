#include "./nbt.h"

#include <string>
#include <list>

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

tag::~tag() {}

tag_byte::tag_byte() : tag(), name(), p() {}
tag_byte::~tag_byte() {
  delete name;
  name = NULL;
}
tag_byte::tag_byte(gzFile* file, bool named)
          : name(named ? new tag_string(file, false) : NULL), p() {
  gzread(*file, reinterpret_cast<void*>(&p), sizeof(p));
}

tag_short::tag_short() : tag(), name(), p() {}
tag_short::~tag_short() {
  delete name;
  name = NULL;
}
tag_short::tag_short(gzFile* file, bool named)
          : name(named ? (new tag_string(file, false)) : NULL), p() {
  gzread(*file, reinterpret_cast<void*>(&p), sizeof(p));
  p = endian_swap<int16_t>(p);
  std::cout << id() << " is: " << ((name != NULL) ? name->p + " " : "")
            << p << " (size: " << sizeof(p) << ")" << std::endl;
}

tag_int::tag_int() : tag(), name(), p() {}
tag_int::~tag_int() {
  delete name;
  name = NULL;
}
tag_int::tag_int(gzFile* file, bool named)
          : name(named ? new tag_string(file, false) : NULL), p() {
  gzread(*file, reinterpret_cast<void*>(&p), sizeof(p));
  p = endian_swap<int32_t>(p);
  std::cout << id() << " is: " << ((name != NULL) ? name->p + " " : "")
            << p << " (size: " << sizeof(p) << ")" << std::endl;
}

tag_long::tag_long() : tag(), name(), p() {}
tag_long::~tag_long() {
  delete name;
  name = NULL;
}
tag_long::tag_long(gzFile* file, bool named)
          : name(named ? new tag_string(file, false) : NULL), p() {
  gzread(*file, reinterpret_cast<void*>(&p), sizeof(p));
  p = endian_swap<int64_t>(p);
  std::cout << id() << " is: " << ((name != NULL) ? name->p + " " : "")
            << p << " (size: " << sizeof(p) << ")" << std::endl;
}

tag_float::tag_float() : tag(), name(), p() {}
tag_float::~tag_float() {
  delete name;
  name = NULL;
}
tag_float::tag_float(gzFile* file, bool named)
          : name(named ? new tag_string(file, false) : NULL), p() {
  gzread(*file, reinterpret_cast<void*>(&p), sizeof(p));
  p = endian_swap<float>(p);
  std::cout << id() << " is: " << ((name != NULL) ? name->p + " " : "")
            << p << " (size: " << sizeof(p) << ")" << std::endl;
}

tag_double::tag_double() : tag(), name(), p() {}
tag_double::~tag_double() {
  delete name;
  name = NULL;
}
tag_double::tag_double(gzFile* file, bool named)
          : name(named ? new tag_string(file, false) : NULL), p() {
  gzread(*file, reinterpret_cast<void*>(&p), sizeof(p));
  p = endian_swap<double>(p);
  std::cout << id() << " is: " << ((name != NULL) ? name->p + " " : "")
            << p << " (size: " << sizeof(p) << ")" << std::endl;
}

tag_byte_array::tag_byte_array() : tag(), name(), length(), p() {}
tag_byte_array::~tag_byte_array() {
  delete name;
  name = NULL;
}
tag_byte_array::tag_byte_array(gzFile* file, bool named)
          : name(named ? new tag_string(file, false) : NULL), length(), p() {
  length = tag_int(file, false);
  char* bufferstring = new char[length.p];
  gzread(*file, bufferstring, length.p);
  p = std::string(bufferstring, length.p);
  delete[] bufferstring;
}

tag_string::tag_string() : tag(), name(), length(), p() {}
tag_string::~tag_string() {
  delete name;
  name = NULL;
}
tag_string::tag_string(gzFile* file, bool named)
          : name(named ? new tag_string(file, false) : NULL), length(), p() {
  length = tag_short(file, false);
  char* bufferstring = new char[length.p];
  gzread(*file, bufferstring, length.p);
  p = std::string(bufferstring, length.p);
  std::cout << p << std::endl;
  delete[] bufferstring;
}

tag_list::tag_list() : tag(), name(), tagid(), length(), tags() {}
tag_list::~tag_list() {
  delete name;
  name = NULL;
  for (std::list<tag*>::iterator i = tags.begin(); i != tags.end();) {
    delete *i++;
  }
}
tag_list::tag_list(gzFile* file, bool named)
          : name(named ? new tag_string(file, false) : NULL),
            tagid(), length(), tags() {
  tagid = tag_byte(file, false);
  length = tag_int(file, false);
  for (int i = 0; i < length.p; ++i) {
    push_in_tags(&tags, file, tagid.p, false);
  }
}

tag_compound::tag_compound() : tag(), name(), tags() {}
tag_compound::~tag_compound() {
  delete name;
  name = NULL;
  for (std::list<tag*>::iterator i = tags.begin(); i != tags.end();) {
    delete *i++;
  }
}
tag_compound::tag_compound(gzFile* file, bool named)
          : name(named ? (new tag_string(file, false)) : NULL), tags() {
  int buffer;
  while ((buffer = gzgetc(*file)) != 0) {
    push_in_tags(&tags, file, buffer, true);
  }
}

void push_in_tags(std::list<tag*>* tags, gzFile* file,
                  int switcher, bool with_string) {
  switch (switcher) {
    case -1:
      std::cerr << "file read error!" << std::endl;
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
      std::cerr << "wrong file format: " << switcher << std::endl;
      exit(1);
      break;
  }
}
}


nbt::nbt(const std::string& filename) : global() {
  gzFile filein = gzopen(filename.c_str(), "rb");
  if (!filein) {
    std::cerr << "file could not be opened!" << std::endl;
    exit(1);
  }

  int buffer = gzgetc(filein);
  switch (buffer) {
    case -1:
      std::cerr << "file read error!" << std::endl;
      exit(1);
      break;
    case 10:
      global = new tag::tag_compound(&filein, true);
      break;
    default:
      std::cerr << "wrong file format!" << std::endl;
      exit(1);
      break;
  }
  gzclose(filein);
}
nbt::~nbt() {
  delete global;
}

std::string nbt::string() {
  return "";
}
