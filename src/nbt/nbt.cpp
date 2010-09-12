#include "./nbt.h"

#include <errno.h>
#include <string>
#include <list>
#include <limits>
#include <sstream>

#include "./colors.h"

nbt::nbt(int world) : tag_(),
                      xPos_min_(std::numeric_limits<int32_t>::max()),
                      zPos_min_(std::numeric_limits<int32_t>::max()),
                      xPos_max_(std::numeric_limits<int32_t>::min()),
                      zPos_max_(std::numeric_limits<int32_t>::min()),
                      dir_(QDir::home()) {
  if (!dir_.cd(QString(".minecraft/saves/World") + QString::number(world))) {
    qFatal("Minecraft is not installed!");
  }
  QString name = "map" + QString::number(world);
  dir_.setFilter(QDir::Files);
  QDirIterator it(dir_, QDirIterator::Subdirectories);
  while (it.hasNext()) {
    it.next();
    std::string fn = it.fileName().toAscii().data();
    size_t first = fn.find(".");
    size_t second = fn.find(".", first + 1);
    if (second != std::string::npos) {
      long x = strtol(&(fn.c_str()[first + 1]), NULL, 36);
      long z = strtol(&(fn.c_str()[second + 1]), NULL, 36);
      xPos_min_ = std::min(static_cast<int32_t>(x), xPos_min_);
      xPos_max_ = std::max(static_cast<int32_t>(x), xPos_max_);
      zPos_min_ = std::min(static_cast<int32_t>(z), zPos_min_);
      zPos_max_ = std::max(static_cast<int32_t>(z), zPos_max_);
    }
  }
  std::cout << "x: " << xPos_min_ << " " << xPos_max_ << std::endl;
  std::cout << "z: " << zPos_min_ << " " << zPos_max_ << std::endl;
}

nbt::nbt(const std::string& filename)
          : tag_(),
            xPos_min_(std::numeric_limits<int32_t>::max()),
            zPos_min_(std::numeric_limits<int32_t>::max()),
            xPos_max_(std::numeric_limits<int32_t>::min()),
            zPos_max_(std::numeric_limits<int32_t>::min()),
            dir_() {
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
      tag_ = tag_ptr(new tag::tag_<tag::compound>(&filein, true));
      break;
    default:
      std::cerr << "wrong file format! " << filename << std::endl;
      exit(1);
      break;
  }
  gzclose(filein);
}

const nbt::tag_ptr nbt::tag_at(int32_t x, int32_t z) {
  QDir tmp = dir_;
  QString dirname(QString::number((x + 64) % 64, 36) + "/"
                + QString::number((z + 64) % 64, 36));
  if (!tmp.cd(dirname)) {
    return tag_ptr();
  }
  QString filename(QString("c.")
                 + ((x < 0) ? "-" : "") + QString::number(abs(x), 36) + "."
                 + ((z < 0) ? "-" : "") + QString::number(abs(z), 36) + ".dat");
  if (!tmp.exists(filename)) {
    return tag_ptr();
  }
  QString dirstring = tmp.filePath(filename);

  gzFile filein = gzopen(dirstring.toAscii().data(), "rb");
  if (!filein) {
    std::cerr << "file could not be opened! "
              << dirstring.toAscii().data() << " "
              << errno << std::endl;
    exit(1);
  }
  int buffer = gzgetc(filein);
  switch (buffer) {
    case -1:
      std::cerr << "file read error! " << std::endl;
      break;
    case 10:
      {
        tag_ptr ret(new tag::tag_<tag::compound>(&filein, true));
        gzclose(filein);
        return ret;
      }
      break;
    default:
      std::cerr << "wrong file format! " << std::endl;
      break;
  }
  if (gzclose(filein) != Z_OK) {
    std::cerr << "could not close file! "
              << dirstring.toAscii().data() << std::endl;
  }
  exit(1);
}

std::string nbt::string() {
  return tag_->str();
}
