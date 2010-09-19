#include "./nbt.h"

#include <errno.h>
#include <zlib.h>

#include <algorithm>
#include <limits>
#include <list>
#include <map>
#include <sstream>
#include <stack>
#include <string>
#include <utility>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

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

void nbt::setSettings(Settings set__) {
  set_ = set__;
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
  int lighter_amount = 0;
  if ((colors[getValue(cache, x - xd, y, z, j, i)].alpha() == 0
    || colors[getValue(cache, x, y, z - zd, j, i)].alpha() == 0)
   && colors[getValue(cache, x - xd, y, z - zd, j, i)].alpha() == 0) {
    lighter_amount += set_.relief_strength;
  }
  if ((colors[getValue(cache, x + xd, y, z, j, i)].alpha() == 0
    || colors[getValue(cache, x, y, z + zd, j, i)].alpha() == 0)
   && colors[getValue(cache, x + xd, y, z + zd, j, i)].alpha() == 0) {
    lighter_amount -= set_.relief_strength;
  }
  if (!set_.topview && lighter_amount < 0) lighter_amount = 0;
  if (lighter_amount < 0) {
    color = color.darker(100 - lighter_amount);
  } else if (lighter_amount > 0) {
    color = color.lighter(100 + lighter_amount);
  }
  return color;
}

QColor nbt::checkReliefNormal(const nbt::map& cache, QColor input, int x, int y, int z,
                                            int j, int i) const {
  QColor color = input;
  int lighter_amount = 0;
  if (set_.sun_direction % 4 == 2) {
    if ((colors[getValue(cache, x + 1, y, z - 1, j, i)].alpha() == 0
      || colors[getValue(cache, x - 1, y, z - 1, j, i)].alpha() == 0)
     && colors[getValue(cache, x, y, z - 1, j, i)].alpha() == 0) {
      if (set_.sun_direction == 2)
        lighter_amount -= set_.relief_strength;
      if (set_.sun_direction == 6)
        lighter_amount += set_.relief_strength;
    }
    if ((colors[getValue(cache, x + 1, y, z + 1, j, i)].alpha() == 0
      || colors[getValue(cache, x - 1, y, z + 1, j, i)].alpha() == 0)
     && colors[getValue(cache, x, y, z + 1, j, i)].alpha() == 0) {
      if (set_.sun_direction == 2)
        lighter_amount += set_.relief_strength;
      if (set_.sun_direction == 6)
        lighter_amount -= set_.relief_strength;
    }
  } else if (set_.sun_direction % 4 == 0) {
    if ((colors[getValue(cache, x - 1, y, z + 1, j, i)].alpha() == 0
      || colors[getValue(cache, x - 1, y, z - 1, j, i)].alpha() == 0)
     && colors[getValue(cache, x - 1, y, z, j, i)].alpha() == 0) {
      if (set_.sun_direction == 4)
        lighter_amount -= set_.relief_strength;
      if (set_.sun_direction == 0)
        lighter_amount += set_.relief_strength;
    }
    if ((colors[getValue(cache, x + 1, y, z - 1, j, i)].alpha() == 0
      || colors[getValue(cache, x + 1, y, z + 1, j, i)].alpha() == 0)
     && colors[getValue(cache, x + 1, y, z, j, i)].alpha() == 0) {
      if (set_.sun_direction == 4)
        lighter_amount += set_.relief_strength;
      if (set_.sun_direction == 0)
        lighter_amount -= set_.relief_strength;
    }
  }
  if (!set_.topview && lighter_amount < 0) lighter_amount = 0;
  if (lighter_amount < 0) {
    color = color.darker(100 - lighter_amount);
  } else if (lighter_amount > 0) {
    color = color.lighter(100 + lighter_amount);
  }
  return color;
}

QColor nbt::calculateShadow(const nbt::map& cache, QColor input, int x, int y, int z,
                                          int j, int i, bool zigzag) const {
  QColor color = input;
  if (set_.shadow) {
    int blockid = getValue(cache, x, y, z, j, i);
    if (!set_.topview) {
      if (zigzag) {
        intmapit it = upperHalf.find(blockid);
        if (it != upperHalf.end()) blockid = (*it).second;
      } else {
        intmapit it = lowerHalf.find(blockid);
        if (it != lowerHalf.end()) blockid = (*it).second;
      }
    }
    if (emitLight.count(blockid) == 1) {
      return color;
    }
    ++y;
    int yy = y;
    int xx = x;
    int zz = z;
    int sun_direction = (set_.sun_direction + ((set_.rotate + 1) % 4) * 2) % 8;
    int shadow_amount;
    if (set_.topview) {
      shadow_amount = 0;
    } else {
      shadow_amount = ((sun_direction == 3 || sun_direction == 4 || sun_direction == 5) ? 0 : 1) * !zigzag * set_.relief_strength * 2;
      int32_t blockid2;
      if (set_.rotate == 0) {
        blockid = getValue(cache, x - 1, y - 1, z, j, i);
        blockid2 = getValue(cache, x + 1, y - 1, z, j, i);
      } else if (set_.rotate == 1) {
        blockid = getValue(cache, x, y - 1, z + 1, j, i);
        blockid2 = getValue(cache, x, y - 1, z - 1, j, i);
      } else if (set_.rotate == 2) {
        blockid = getValue(cache, x + 1, y - 1, z, j, i);
        blockid2 = getValue(cache, x - 1, y - 1, z, j, i);
      } else {
        blockid = getValue(cache, x, y - 1, z - 1, j, i);
        blockid2 = getValue(cache, x, y - 1, z + 1, j, i);
      }
      if ((sun_direction == 1 &&
           colors[blockid].alpha() == 0)
       || (sun_direction == 2)
       || (sun_direction == 6)
       || (sun_direction == 7 &&
           colors[blockid2].alpha() == 0)) {
        shadow_amount /= 2;
      }
    }
    zigzag = true;
    for (int iii = 0; iii < set_.shadow_quality + 1; ++iii) {
      QColor light(0, 0, 0, 0);
      while (y < 127) {
        if (!zigzag) {
          ++y;
          zigzag = true;
        } else {
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
          zigzag = false;
        }
        uint8_t blknr = getValue(cache, x, y, z, j, i);
        if (noShadow.count(blknr) == 0) {
          light = blend(colors[blknr], light);
          if (light.alpha() == 255) {
            break;
          }
        }
      }
      y = yy;
      z = zz;
      x = xx;
      zigzag = false;
      shadow_amount += static_cast<int>(set_.shadow_strength
                     / (set_.shadow_quality + 1) * light.alphaF());
    }
    color = color.darker(100 + shadow_amount);
  }
  return color;
}

QColor nbt::calculateRelief(const nbt::map& cache, QColor input, int x, int y, int z,
                                          int j, int i) const {
  QColor color = input;
  //++y;
  if (set_.relief) {
    if (set_.sun_direction % 2 == 1) {
      color = checkReliefDiagonal(cache, color, x, y, z, j, i);
    } else {
      color = checkReliefNormal(cache, color, x, y, z, j, i);
    }
  }
  return color;
}

QColor nbt::calculateMap(const nbt::map& cache, QColor input, int x, int y, int z,
                                       int j, int i, bool zigzag) const {
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
        color = blend(calculateShadow(cache, colors[blknr], x, h, z, j, i), color);
      }
    }
  } else if (set_.oblique) {
    std::stack<QColor> colorstack;
    int& dec = (set_.rotate % 2 == 0) ? z : x;
    bool first_block_hit = false;
    do {
      int32_t blockid = getValue(cache, x, y, z, j, i);
      if (zigzag) {
        intmapit it = upperHalf.find(blockid);
        if (it != upperHalf.end()) blockid = (*it).second;
      } else {
        intmapit it = lowerHalf.find(blockid);
        if (it != lowerHalf.end()) blockid = (*it).second;
      }
      if (set_.shadow_quality_ultra || !first_block_hit) {
        colorstack.push(calculateShadow(cache, colors[blockid], x, y, z, j, i, zigzag));
        first_block_hit = true;
      } else {
        colorstack.push(colors[blockid]);
      }
      if (zigzag) {
        if (set_.rotate <= 1) {
          --dec;
        } else {
          ++dec;
        }
        zigzag = false;
      } else {
        --y;
        zigzag = true;
      }
      if (!colorstack.empty() && colorstack.top().alpha() == 255) break;
      if (dec == -1 || dec == 16) {
        std::stack<QColor> colorstack_inner = colorstack;
        bool clear = true;
        while (!colorstack.empty()) {
          if (colorstack.top().alpha() != 0) clear = false;
          colorstack.pop();
        }
        if (clear) return QColor(Qt::transparent);
        colorstack = colorstack_inner;
      }
    } while (y >= 0);
    QColor tmp(Qt::transparent);
    while (!colorstack.empty()) {
      tmp = blend(colorstack.top(), tmp);
      colorstack.pop();
    }
    color = blend(color, tmp);
  }
  return color;
}

inline int clamp(int value) {
  return (value > 255) ? 255 : ((value < 0) ? 0 : value);
}

QImage nbt::getImage(int32_t j, int32_t i, bool* result) const {
  QImage img;
  if (set_.topview)
    img = QImage(16, 16, QImage::Format_ARGB32_Premultiplied);
  else if (set_.oblique)
    img = QImage(16, 16 + 128, QImage::Format_ARGB32_Premultiplied);
  else
    exit(1);
  img.fill(0);
  const nbt::tag_ptr tag = tag_at(j, i);
  if (tag) {
    boost::mt19937 gen((((j - std::numeric_limits<int16_t>::min()) << 16) & 0xFFFF0000)
                      | ((i - std::numeric_limits<int16_t>::min())        & 0x0000FFFF));
    boost::uniform_int<> dist(-1, 1);
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> > dither(gen, dist);
    nbt::tag_ptr comp(tag->sub("Level"));
    int32_t xPos = comp->sub("xPos")->pay_<int32_t>();
    int32_t zPos = comp->sub("zPos")->pay_<int32_t>();
    if (zPos != i || xPos != j) {
      std::cerr << "wrong tag in getImage!" << std::endl;
    }
    for (int jj = j + 7; jj >= j - 7; --jj) {
      for (int ii = i + 7; ii >= i - 7; --ii) {
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
    if (set_.topview) {
      for (int32_t zz = 0; zz < 16; ++zz) {
        for (int32_t xx = 0; xx < 16; ++xx) {
          uint32_t x, z;
          if (set_.rotate == 0) {
            x = xx;
            z = zz;
          } else if (set_.rotate == 1) {
            x = zz;
            z = 15 - xx;
          } else if (set_.rotate == 2) {
            x = 15 - xx;
            z = 15 - zz;
          } else {
            x = 15 - zz;
            z = xx;
          }
          uint32_t y = 127;
          while (getValue(cache, x, y, z, j, i) == 0) {
            --y;
          }
          QColor color(Qt::transparent);
          color = calculateMap(cache, color, x, y, z, j, i);
          color = calculateRelief(cache, color, x, y, z, j, i);
          color.lighter((y - 64) / 2 + 96);
          if (set_.dither) {
            int random1 = dither();
            int random2 = dither();
            color.setRed(clamp(color.red() + random1 + random2));
            color.setGreen(clamp(color.green() + random1 + random2));
            color.setBlue(clamp(color.blue() + random1 + random2));
          }
          img.setPixel(xx, zz, color.rgba());
        }
      }
    } else if (set_.oblique) {
      /* iterate over image pixels */
      for (int32_t zz = 0; zz < 16 + 128; ++zz) {
        for (int32_t xx = 0; xx < 16; ++xx) {
          int32_t x, y, z;
          if (set_.rotate == 0) {
            x = xx;
            y = 143 - zz;
            if (y > 127) y = 127;
            z = zz;
            if (zz > 15) z = 15;
          } else if (set_.rotate == 1) {
            z = 15 - xx;
            y = 143 - zz;
            if (y > 127) y = 127;
            x = zz;
            if (zz > 15) x = 15;
          } else if (set_.rotate == 2) {
            z = 15 - zz;
            y = 143 - zz;
            if (y > 127) y = 127;
            x = 15 - xx;
            if (zz > 15) z = 0;
          } else {
            z = xx;
            y = 143 - zz;
            if (y > 127) y = 127;
            x =  15 - zz;
            if (zz > 15) x = 0;
          }
          /* at this point x, y and z are block coordinates */
          bool zigzag = (zz > 15) ? false : true;
          int& dec = (set_.rotate % 2 == 0) ? z : x;
          while (getValue(cache, x, y, z, j, i) == 0) {
            if (zigzag) {
              if (set_.rotate <= 1) {
                --dec;
              } else {
                ++dec;
              }
              zigzag = false;
            } else {
              --y;
              zigzag = true;
            }
            if (y < 0 || dec < 0 || dec > 15) {
              goto endloop;
            }
          }
          {
            QColor color(Qt::transparent);
            color = calculateMap(cache, color, x, y, z, j, i, zigzag);
            color = calculateRelief(cache, color, x, y, z, j, i);
            color = color.lighter((y - 64) / 2 + 96);
            if (set_.dither) {
              int random1 = dither();
              int random2 = dither();
              color.setRed(clamp(color.red() + random1 + random2));
              color.setGreen(clamp(color.green() + random1 + random2));
              color.setBlue(clamp(color.blue() + random1 + random2));
            }
            img.setPixel(xx, zz, color.rgba());
          }
          endloop:;
        }
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
