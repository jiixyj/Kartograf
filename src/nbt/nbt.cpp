#include "./nbt.h"

#include <errno.h>
#include <zlib.h>

#include <algorithm>
#include <limits>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <utility>

#include "./colors.h"

nbt::nbt() : tag_(),
             xPos_min_(std::numeric_limits<int32_t>::max()),
             zPos_min_(std::numeric_limits<int32_t>::max()),
             xPos_max_(std::numeric_limits<int32_t>::min()),
             zPos_max_(std::numeric_limits<int32_t>::min()),
             dir_(QDir::home()),
             set_(),
             blockcache_() {}

nbt::nbt(int world) : tag_(),
                      xPos_min_(std::numeric_limits<int32_t>::max()),
                      zPos_min_(std::numeric_limits<int32_t>::max()),
                      xPos_max_(std::numeric_limits<int32_t>::min()),
                      zPos_max_(std::numeric_limits<int32_t>::min()),
                      dir_(QDir::home()),
                      set_(),
                      blockcache_() {
  if (!dir_.cd(QString(".minecraft/saves/World") + QString::number(world))) {
    qFatal("Minecraft is not installed!");
  }
  QString name = "map" + QString::number(world);
  dir_.setFilter(QDir::Files);
  construct_world();
}

nbt::nbt(const std::string& filename)
          : tag_(),
            xPos_min_(std::numeric_limits<int32_t>::max()),
            zPos_min_(std::numeric_limits<int32_t>::max()),
            xPos_max_(std::numeric_limits<int32_t>::min()),
            zPos_max_(std::numeric_limits<int32_t>::min()),
            dir_(),
            set_(),
            blockcache_() {
  if ((dir_ = QDir(QString::fromStdString(filename))).exists()) {
    construct_world();
    return;
  }
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

void nbt::construct_world() {
  QDirIterator it(dir_, QDirIterator::Subdirectories);
  while (it.hasNext()) {
    it.next();
    std::string fn = it.fileName().toAscii().data();
    size_t first = fn.find(".");
    size_t second = fn.find(".", first + 1);
    if (second != std::string::npos) {
      int64_t x = strtol(&(fn.c_str()[first + 1]), NULL, 36);
      int64_t z = strtol(&(fn.c_str()[second + 1]), NULL, 36);
      xPos_min_ = std::min(static_cast<int32_t>(x), xPos_min_);
      xPos_max_ = std::max(static_cast<int32_t>(x), xPos_max_);
      zPos_min_ = std::min(static_cast<int32_t>(z), zPos_min_);
      zPos_max_ = std::max(static_cast<int32_t>(z), zPos_max_);
    }
  }
  std::cout << "x: " << xPos_min_ << " " << xPos_max_ << std::endl;
  std::cout << "z: " << zPos_min_ << " " << zPos_max_ << std::endl;
}


const nbt::tag_ptr nbt::tag_at(int32_t x, int32_t z) const {
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

void nbt::setSettings(Settings set) {
  set_ = set;
  return;
}

uint8_t nbt::getValue(const nbt::map& cache,
                 int32_t x, int32_t y, int32_t z, int32_t j, int32_t i) const {
  while (x < 0) {
    --j;
    x += 16;
  }
  while (x > 15) {
    ++j;
    x -= 16;
  }
  while (z < 0) {
    --i;
    z += 16;
  }
  while (z > 15) {
    ++i;
    z -= 16;
  }
  nbt::map::const_accessor acc;
  if (cache.find(acc, std::pair<int, int>(j, i))) {
    return acc->second[y + z * 128 + x * 128 * 16];
  } else {
    return 0;
  }
}

QColor nbt::checkReliefDiagonal(const nbt::map& cache, QColor input, int x, int y, int z,
                                              int j, int i) const {
  int xd = 0, zd = 0;
  QColor color = input;
  if (set_.sun_direction == 7 || set_.sun_direction == 1) {
    ++xd;
  } else if (set_.sun_direction == 3 || set_.sun_direction == 5) {
    --xd;
  }
  if (set_.sun_direction == 7 || set_.sun_direction == 5) {
    ++zd;
  } else if (set_.sun_direction == 3 || set_.sun_direction == 1) {
    --zd;
  }
  if ((colors[getValue(cache, x + xd, y, z, j, i)].alpha() == 255
    || colors[getValue(cache, x, y, z + zd, j, i)].alpha() == 255)
   && colors[getValue(cache, x + xd, y, z + zd, j, i)].alpha() == 255) {
      color = color.lighter(120);
  }
  if ((colors[getValue(cache, x - xd, y, z, j, i)].alpha() == 255
    || colors[getValue(cache, x, y, z - zd, j, i)].alpha() == 255)
   && colors[getValue(cache, x - xd, y, z - zd, j, i)].alpha() == 255) {
      color = color.darker(120);
  }
  return color;
}

QColor nbt::checkReliefNormal(const nbt::map& cache, QColor input, int x, int y, int z,
                                            int j, int i) const {
  QColor color = input;
  if (set_.sun_direction % 4 == 2) {
    if ((colors[getValue(cache, x + 1, y, z - 1, j, i)].alpha() == 255
      || colors[getValue(cache, x - 1, y, z - 1, j, i)].alpha() == 255)
     && colors[getValue(cache, x, y, z - 1, j, i)].alpha() == 255) {
      if (set_.sun_direction == 2)
        color = color.lighter(120);
      if (set_.sun_direction == 6)
        color = color.darker(120);
    }
    if ((colors[getValue(cache, x + 1, y, z + 1, j, i)].alpha() == 255
      || colors[getValue(cache, x - 1, y, z + 1, j, i)].alpha() == 255)
     && colors[getValue(cache, x, y, z + 1, j, i)].alpha() == 255) {
      if (set_.sun_direction == 2)
        color = color.darker(120);
      if (set_.sun_direction == 6)
        color = color.lighter(120);
    }
  } else if (set_.sun_direction % 4 == 0) {
    if ((colors[getValue(cache, x - 1, y, z + 1, j, i)].alpha() == 255
      || colors[getValue(cache, x - 1, y, z - 1, j, i)].alpha() == 255)
     && colors[getValue(cache, x - 1, y, z, j, i)].alpha() == 255) {
      if (set_.sun_direction == 4)
        color = color.lighter(120);
      if (set_.sun_direction == 0)
        color = color.darker(120);
    }
    if ((colors[getValue(cache, x + 1, y, z - 1, j, i)].alpha() == 255
      || colors[getValue(cache, x + 1, y, z + 1, j, i)].alpha() == 255)
     && colors[getValue(cache, x + 1, y, z, j, i)].alpha() == 255) {
      if (set_.sun_direction == 4)
        color = color.darker(120);
      if (set_.sun_direction == 0)
        color = color.lighter(120);
    }
  }
  return color;
}

QColor nbt::calculateShadow(const nbt::map& cache, QColor input, int x, int y, int z,
                                          int j, int i) const {
  QColor color = input;
  if (set_.shadow) {
    QColor light(0, 0, 0, 0);
    while (++y < 128) {
      if (set_.sun_direction == 7
       || set_.sun_direction == 0
       || set_.sun_direction == 1) {
        --x;
      } else if (set_.sun_direction == 3
              || set_.sun_direction == 4
              || set_.sun_direction == 5) {
        ++x;
      }
      if (set_.sun_direction == 5
       || set_.sun_direction == 6
       || set_.sun_direction == 7) {
        --z;
      } else if (set_.sun_direction == 1
              || set_.sun_direction == 2
              || set_.sun_direction == 3) {
        ++z;
      }
      uint8_t blknr = getValue(cache, x, y, z, j, i);
      if (blknr != 0) {
        light = blend(colors[blknr], light);
        if (light.alpha() == 255) {
          break;
        }
      }
    }
    color = color.darker(static_cast<int>(50.0 * light.alphaF()) + 100);
  }
  return color;
}

QColor nbt::calculateRelief(const nbt::map& cache, QColor input, int x, int y, int z,
                                          int j, int i) const {
  QColor color = input;
  if (set_.topview) {
    if (set_.relief) {
      if (set_.sun_direction % 2 == 1) {
        color = checkReliefDiagonal(cache, color, x, y, z, j, i);
      } else {
        color = checkReliefNormal(cache, color, x, y, z, j, i);
      }
    }
  }
  return color;
}

QColor nbt::calculateMap(const nbt::map& cache, QColor input, int x, int y, int z,
                                       int j, int i) const {
  QColor color = input;
  if (set_.topview) {
    if (set_.heightmap) {
      if (set_.color) {
        color.setHsvF(atan(((1.0 - y / 127.0) - 0.5) * 10) / M_PI + 0.5,
                      1.0, 1.0, 1.0);
      } else {
        color.setRgba(QColor(y, y, y, 255).rgba());
      }
    } else {
      int height_low_bound = y;
      while (colors[getValue(cache, x, height_low_bound--, z, j, i)]
                                                               .alpha() != 255);
      for (int h = height_low_bound; h <= y; ++h) {
        uint8_t blknr = getValue(cache, x, h, z, j, i);
        color = blend(colors[blknr], color);
      }
    }
  }
  return color;
}

QImage nbt::getImage(int32_t j, int32_t i, bool* result) const {
  QImage img(16, 16, QImage::Format_ARGB32_Premultiplied);
  img.fill(0);
  const nbt::tag_ptr tag = tag_at(j, i);
  if (tag) {
    nbt::tag_ptr comp(tag->sub("Level"));
    int32_t xPos = comp->sub("xPos")->pay_<int32_t>();
    int32_t zPos = comp->sub("zPos")->pay_<int32_t>();
    if (zPos != i || xPos != j) {
      std::cerr << "wrong tag in getImage!" << std::endl;
    }
    const std::string& heightMap = comp->sub("HeightMap")->
                                         pay_<tag::byte_array>().p;
    for (int jj = j + 3; jj >= j - 3; --jj) {
      for (int ii = i + 3; ii >= i - 3; --ii) {
        if (blockcache_.count(std::pair<int, int>(jj, ii)) == 0) {
          const nbt::tag_ptr newtag = tag_at(jj, ii);
          if (newtag) {
            const std::string& pl = newtag->sub("Level")->
                                       sub("Blocks")->pay_<tag::byte_array>().p;
            nbt::map::accessor acc;
            blockcache_.insert(acc, std::pair<int, int>(jj, ii));
            acc->second = pl;
          }
        }
      }
    }
    nbt::map cache(blockcache_);
    uint64_t xtmp = (xPos - xPos_min()) * 16;
    uint64_t ztmp = (zPos - zPos_min()) * 16;
    int32_t max_int = std::numeric_limits<int32_t>::max();
    if (xtmp + 15 > static_cast<uint64_t>(max_int)
     || ztmp + 15 > static_cast<uint64_t>(max_int)) {
      std::cerr << "Map is too large for an image!" << std::endl;
      exit(1);
    }
    int32_t xPos_img = static_cast<int32_t>(xtmp);
    int32_t zPos_img = static_cast<int32_t>(ztmp);
    int index = 0;
    for (int32_t ii = zPos_img; ii < zPos_img + 16; ++ii) {
      for (int32_t jj = xPos_img; jj < xPos_img + 16; ++jj) {
        int32_t x = jj - xPos_img;
        int32_t z = ii - zPos_img;
        uint8_t y = heightMap[index++];
        QColor color;
        color = calculateMap(cache, color, x, y, z, j, i);
        color = calculateShadow(cache, color, x, y, z, j, i);
        color = calculateRelief(cache, color, x, y, z, j, i);
        img.setPixel(x, z, color.lighter((y - 64) / 2 + 64).rgba());
      }
    }
    *result = true;
  } else {
    *result = false;
  }
  return img;
}

void nbt::clearCache() const {
  blockcache_.clear();
}

std::string nbt::string() const {
  return (tag_ != 0) ? tag_->str() : "";
}
