#include "./nbt.h"

namespace tag {

template <typename T>
T endian_swap(T d) {
  T a = d;
  unsigned char* dst = (unsigned char*) &a;
  unsigned char* src = (unsigned char*) &d;

  for (int i = 0; i < sizeof(T); ++i) {
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

tag::~tag() {}

tag_byte::tag_byte() : tag(), name(), p() {}
tag_byte::tag_byte(gzFile* file, bool named)
          : name(named ? new tag_string(file, false) : NULL), p() {
  gzread(*file, reinterpret_cast<void*>(&p), sizeof(p));
}

tag_short::tag_short() : tag(), name(), p() {}
tag_short::tag_short(gzFile* file, bool named)
          : name(named ? (new tag_string(file, false)) : NULL), p() {
  gzread(*file, reinterpret_cast<void*>(&p), sizeof(p));
  p = endian_swap<int16_t>(p);
  std::cout << id() << " is: " << ((name != NULL) ? name->p + " " : "") << p << " (size: " << sizeof(p) << ")" << std::endl;
}

tag_int::tag_int() : tag(), name(), p() {}
tag_int::tag_int(gzFile* file, bool named)
          : name(named ? new tag_string(file, false) : NULL), p() {
  gzread(*file, reinterpret_cast<void*>(&p), sizeof(p));
  p = endian_swap<int32_t>(p);
  std::cout << id() << " is: " << ((name != NULL) ? name->p + " " : "") << p << " (size: " << sizeof(p) << ")" << std::endl;
}

tag_long::tag_long() : tag(), name(), p() {}
tag_long::tag_long(gzFile* file, bool named)
          : name(named ? new tag_string(file, false) : NULL), p() {
  gzread(*file, reinterpret_cast<void*>(&p), sizeof(p));
  p = endian_swap<int64_t>(p);
  std::cout << id() << " is: " << ((name != NULL) ? name->p + " " : "") << p << " (size: " << sizeof(p) << ")" << std::endl;
}

tag_float::tag_float() : tag(), name(), p() {}
tag_float::tag_float(gzFile* file, bool named)
          : name(named ? new tag_string(file, false) : NULL), p() {
  gzread(*file, reinterpret_cast<void*>(&p), sizeof(p));
  p = endian_swap<float>(p);
  std::cout << id() << " is: " << ((name != NULL) ? name->p + " " : "") << p << " (size: " << sizeof(p) << ")" << std::endl;
}

tag_double::tag_double() : tag(), name(), p() {}
tag_double::tag_double(gzFile* file, bool named)
          : name(named ? new tag_string(file, false) : NULL), p() {
  gzread(*file, reinterpret_cast<void*>(&p), sizeof(p));
  p = endian_swap<double>(p);
  std::cout << id() << " is: " << ((name != NULL) ? name->p + " " : "") << p << " (size: " << sizeof(p) << ")" << std::endl;
}

tag_byte_array::tag_byte_array() : tag(), name(), length(), p() {}
tag_byte_array::tag_byte_array(gzFile* file, bool named)
          : name(named ? new tag_string(file, false) : NULL), length(), p() {
  length = tag_int(file, false);
  char* bufferstring = new char[length.p];
  gzread(*file, bufferstring, length.p);
  p = std::string(bufferstring, length.p);
  delete[] bufferstring;
}

tag_string::tag_string() : tag(), name(), length(), p() {}
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
tag_list::tag_list(gzFile* file, bool named)
          : name(named ? new tag_string(file, false) : NULL),
            tagid(), length(), tags() {
  tagid = tag_byte(file, false);
  length = tag_int(file, false);
  int buffer;
  for (int i = 0; i < length.p; ++i) {
    switch (tagid.p) {
      case -1:
        std::cerr << "file read error!" << std::endl;
        exit(1);
        break;
      case 1:
        tags.push_back(new tag_byte(file, false));
        break;
      case 2:
        tags.push_back(new tag_short(file, false));
        break;
      case 3:
        tags.push_back(new tag_int(file, false));
        break;
      case 4:
        tags.push_back(new tag_long(file, false));
        break;
      case 5:
        tags.push_back(new tag_float(file, false));
        break;
      case 6:
        tags.push_back(new tag_double(file, false));
        break;
      case 7:
        tags.push_back(new tag_byte_array(file, false));
        break;
      case 8:
        tags.push_back(new tag_string(file, false));
        break;
      case 9:
        tags.push_back(new tag_list(file, false));
        break;
      case 10:
        tags.push_back(new tag_compound(file, false));
        break;
      default:
        std::cerr << "wrong file format: " << buffer << std::endl;
        exit(1);
        break;
    }
  }
}

tag_compound::tag_compound() : tag(), name(), tags() {}
tag_compound::tag_compound(gzFile* file, bool named)
          : name(named ? new tag_string(file, false) : NULL), tags() {
  int buffer;
  while ((buffer = gzgetc(*file)) != 0) {
    switch (buffer) {
      case -1:
        std::cerr << "file read error!" << std::endl;
        exit(1);
        break;
      case 1:
        tags.push_back(new tag_byte(file, true));
        break;
      case 2:
        tags.push_back(new tag_short(file, true));
        break;
      case 3:
        tags.push_back(new tag_int(file, true));
        break;
      case 4:
        tags.push_back(new tag_long(file, true));
        break;
      case 5:
        tags.push_back(new tag_float(file, true));
        break;
      case 6:
        tags.push_back(new tag_double(file, true));
        break;
      case 7:
        tags.push_back(new tag_byte_array(file, true));
        break;
      case 8:
        tags.push_back(new tag_string(file, true));
        break;
      case 9:
        tags.push_back(new tag_list(file, true));
        break;
      case 10:
        tags.push_back(new tag_compound(file, true));
        break;
      default:
        std::cerr << "wrong file format: " << buffer << std::endl;
        exit(1);
        break;
    }
  }
  //exit(1);
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
      global = tag::tag_compound(&filein, true);
      break;
    default:
      std::cerr << "wrong file format!" << std::endl;
      exit(1);
      break;
  }
  //databuffer = new unsigned char[length];
  ////filein.seekg (0, std::ios::beg);
  //int q = gzread(filein, databuffer, length);
  //while (q == -1) {
  //  q = gzread(filein, databuffer, length);
  //};
  gzclose(filein);
}

std::string nbt::string() {
  return "";
}
