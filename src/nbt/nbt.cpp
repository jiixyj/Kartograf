#include "./nbt.h"

#include <QtCore>

#include <string>
#include <list>
#include <limits>
#include <sstream>

nbt::nbt(int world) : global() {
  QDir dir = QDir::home();
  if (!dir.cd(QString(".minecraft/saves/World") + QString::number(world))) {
    qFatal("Minecraft is not installed!");
  }
  QString name = "map" + QString::number(world);
  dir.setFilter(QDir::Files);
  QDirIterator it(dir, QDirIterator::Subdirectories);
  int32_t xPos_min = std::numeric_limits<int32_t>::max();
  int32_t zPos_min = std::numeric_limits<int32_t>::max();
  int32_t xPos_max = std::numeric_limits<int32_t>::min();
  int32_t zPos_max = std::numeric_limits<int32_t>::min();
  while (it.hasNext()) {
    it.next();
    // tag::filename = it.next().toAscii().data();
    std::string fn = it.fileName().toAscii().data();
    size_t first = fn.find(".");
    size_t second = fn.find(".", first + 1);
    if (second != std::string::npos) {
      long x = strtol(&(fn.c_str()[first + 1]), NULL, 36);
      long z = strtol(&(fn.c_str()[second + 1]), NULL, 36);
      xPos_min = std::min(static_cast<int32_t>(x), xPos_min);
      xPos_max = std::max(static_cast<int32_t>(x), xPos_max);
      zPos_min = std::min(static_cast<int32_t>(z), zPos_min);
      zPos_max = std::max(static_cast<int32_t>(z), zPos_max);
      // std::cout << tag::filename << "  " << x << " " << z << std::endl;
    }
    // gzFile filein = gzopen(tag::filename.c_str(), "rb");
    // if (!filein) {
    //   std::cerr << "file could not be opened! " << tag::filename << std::endl;
    //   exit(1);
    // }
    // int buffer = gzgetc(filein);
    // switch (buffer) {
    //   case -1:
    //     std::cerr << "file read error! " << tag::filename << std::endl;
    //     gzclose(filein);
    //     continue;
    //   case 10:
    //     // global.push_back(std::tr1::shared_ptr<tag::tag_<tag::compound> >
    //     //                          (new tag::tag_<tag::compound>(&filein, true)));
    //     break;
    //   default:
    //     std::cerr << "wrong file format! " << tag::filename << std::endl;
    //     gzclose(filein);
    //     continue;
    // }
    // gzclose(filein);
  }
  std::cout << "x: " << xPos_min << " " << xPos_max << std::endl;
  std::cout << "z: " << zPos_min << " " << zPos_max << std::endl;
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
