#include "./nbt.h"

#include <errno.h>
#include <string>
#include <list>
#include <limits>
#include <sstream>

#include "./colors.h"

nbt::nbt() : tag_(),
                      xPos_min_(std::numeric_limits<int32_t>::max()),
                      zPos_min_(std::numeric_limits<int32_t>::max()),
                      xPos_max_(std::numeric_limits<int32_t>::min()),
                      zPos_max_(std::numeric_limits<int32_t>::min()),
                      dir_(QDir::home()) {}

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
  construct_world();
}

nbt::nbt(const std::string& filename)
          : tag_(),
            xPos_min_(std::numeric_limits<int32_t>::max()),
            zPos_min_(std::numeric_limits<int32_t>::max()),
            xPos_max_(std::numeric_limits<int32_t>::min()),
            zPos_max_(std::numeric_limits<int32_t>::min()),
            dir_() {
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

uint8_t getValue(const std::map<std::pair<int, int>, std::string>& cache,
              int32_t x, int32_t y, int32_t z, int32_t j, int32_t i) {
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
  std::map<std::pair<int, int>, std::string>::const_iterator it;
  it = cache.find(std::pair<int, int>(j, i));
  if (it != cache.end()) {
    return it->second[y + z * 128 + x * 128 * 16];
  } else {
    return 0;
  }
}

QImage nbt::getImage(int32_t j, int32_t i) const {
  QImage img(16, 16, QImage::Format_ARGB32_Premultiplied);
  img.fill(0);
  const nbt::tag_ptr tag = tag_at(j, i);
  if (tag) {
    nbt::tag_ptr comp(tag->sub("Level"));
    int32_t xPos = comp->sub("xPos")->pay_<int32_t>();
    int32_t zPos = comp->sub("zPos")->pay_<int32_t>();
    const std::string& heightMap = comp->sub("HeightMap")->
                                         pay_<tag::byte_array>().p;
    const std::string& blocks = comp->sub("Blocks")->
                                         pay_<tag::byte_array>().p;
    const std::string& skylight = comp->sub("SkyLight")->
                                         pay_<tag::byte_array>().p;

    for (int jj = j + 3; jj >= j - 3; --jj) {
      for (int ii = i + 3; ii >= i - 3; --ii) {
        if (blockcache_.count(std::pair<int, int>(jj, ii)) == 0) {
          const nbt::tag_ptr newtag = tag_at(jj, ii);
          if (newtag) {
            blockcache_[std::pair<int, int>(jj, ii)] = newtag->sub("Level")->sub("Blocks")->
                                                    pay_<tag::byte_array>().p;
          }
        }
      }
    }

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
        int32_t ii0 = ii - zPos_img;
        int32_t jj0 = jj - xPos_img;
        uint8_t height = heightMap[index++];
        QColor color;
        if (set_.heightmap) {
          if (set_.color) {
            color.setHsvF(atan(((1.0 - height / 127.0) - 0.5) * 10) / M_PI + 0.5, 1.0, 1.0, 1.0);
          } else {
            color.setRgba(QColor(height, height, height, 255).rgba());
          }
        } else {
          int height_low_bound = height;
          while (colors[blocks[height_low_bound-- + ii0 * 128
                                       + jj0 * 128 * 16]].alpha() != 255);
          for (int h = height_low_bound; h <= height; ++h) {
            uint8_t blknr = blocks[h + ii0 * 128 + jj0 * 128 * 16];
            color = blend(colors[blknr], color);
          }
          // painter.setPen(color.lighter((height - 64) / 2 + 64));
          // painter.drawPoint(static_cast<int32_t>(jj), static_cast<int32_t>(ii));
          // label.repaint();
          // label.update();
        }
        if (set_.shadow) {
          int32_t x = jj0, y = height, z = ii0;
          QColor light(0, 0, 0, 0);
          while (++y < 128) {
            --x;
            --z;
            uint8_t blknr = getValue(blockcache_, x, y, z, j, i);
            if (blknr != 0) {
              light = blend(colors[blknr], light);
              if (light.alpha() == 255) {
                break;
              }
            }
          }
          color = color.darker(50.0 * light.alphaF() + 100);
        }
        if (set_.relief) {
          if ((colors[getValue(blockcache_, jj0 + 1, height, ii0, j, i)].alpha() == 255
            || colors[getValue(blockcache_, jj0, height, ii0 + 1, j, i)].alpha() == 255)
           && colors[getValue(blockcache_, jj0 + 1, height, ii0 + 1, j, i)].alpha() == 255) {
            color = color.lighter(120);
          }
          if ((colors[getValue(blockcache_, jj0 - 1, height, ii0, j, i)].alpha() == 255
            || colors[getValue(blockcache_, jj0, height, ii0 - 1, j, i)].alpha() == 255)
           && colors[getValue(blockcache_, jj0 - 1, height, ii0 - 1, j, i)].alpha() == 255) {
            color = color.darker(120);
          }
        }
        img.setPixel(static_cast<int32_t>(jj0), static_cast<int32_t>(ii0),
                     color.lighter((height - 64) / 2 + 64).rgba());
        // uint8_t light = skylight[(height + ii0 * 128 + jj0 * 128 * 16) / 2];
        // if (height % 2 == 1) {
        //   light >>= 4;
        // } else {
        //   light &= 0x0F;
        // }
        // light <<= 4;

      }
    }
    // std::cout << j << " " << xPos << "  "
    //           << i << " " << zPos << std::endl;
  }
  return img;
}

void nbt::clearCache() const {
  blockcache_.clear();
}

std::string nbt::string() {
  return (tag_ != 0) ? tag_->str() : "";
}
