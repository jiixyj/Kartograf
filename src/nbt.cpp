#include "./nbt.h"

#include <boost/math/constants/constants.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

namespace bf = boost::filesystem;

std::string itoa(int value, int base) {
  if (value < 0) {
    throw std::runtime_error("This itoa only supports positive integers!");
  } else if (value == 0) return "0";
  std::string chars = "0123456789abcdefghijklmnopqrstuvwxyz";
  std::string ret;
  do {
    ret.push_back(chars[static_cast<size_t>(value % base)]);
    value /= base;
  } while (value != 0);
  std::reverse(ret.begin(), ret.end());
  return ret;
}

nbt::nbt(): tag_(),
            xPos_min_(std::numeric_limits<int32_t>::max()),
            zPos_min_(std::numeric_limits<int32_t>::max()),
            xPos_max_(std::numeric_limits<int32_t>::min()),
            zPos_max_(std::numeric_limits<int32_t>::min()),
            dir_(getenv("HOME")),
            set_(),
            cache_mutex_(new boost::mutex),
            blockcache_() {}

nbt::nbt(int world)
          : tag_(),
            xPos_min_(std::numeric_limits<int32_t>::max()),
            zPos_min_(std::numeric_limits<int32_t>::max()),
            xPos_max_(std::numeric_limits<int32_t>::min()),
            zPos_max_(std::numeric_limits<int32_t>::min()),
            dir_(),
            set_(),
            cache_mutex_(new boost::mutex),
            blockcache_() {
  char* home_dir;
  if ((home_dir = getenv("HOME"))) {
  } else if ((home_dir = getenv("APPDATA"))) {
  } else {
    throw std::runtime_error("Broken environent!");
  }
  std::stringstream ss;
  ss << "World" << world;
  if (bf::exists(dir_ = bf::path(home_dir) / ".minecraft/saves/" / ss.str())) {
    construct_world();
  } else if (bf::exists(dir_ = bf::path(home_dir) / "Library/"
                          "Application Support/minecraft/saves/" / ss.str())) {
    construct_world();
  } else {
    throw std::runtime_error("Minecraft is not installed!");
  }
}

nbt::nbt(const std::string& filename)
          : tag_(),
            xPos_min_(std::numeric_limits<int32_t>::max()),
            zPos_min_(std::numeric_limits<int32_t>::max()),
            xPos_max_(std::numeric_limits<int32_t>::min()),
            zPos_max_(std::numeric_limits<int32_t>::min()),
            dir_(),
            set_(),
            cache_mutex_(new boost::mutex),
            blockcache_() {
  if (bf::is_directory(bf::status(dir_ = filename))) {
    construct_world();
    return;
  }
  tag::filename = filename;
  gzFile filein = gzopen(filename.c_str(), "rb");
  if (!filein) {
    throw std::runtime_error("file could not be opened! " + filename);
  }

  int buffer = gzgetc(filein);
  switch (buffer) {
    case -1:
      throw std::runtime_error("file read error! " + filename);
      break;
    case 10:
      tag_ = tag_ptr(new tag::tag_<tag::compound>(&filein, true));
      break;
    default:
      throw std::runtime_error("wrong file format! " + filename);
      break;
  }
  gzclose(filein);
}

void nbt::construct_world() {
  bf::recursive_directory_iterator end_itr;
  for (bf::recursive_directory_iterator itr(dir_); itr != end_itr; ++itr) {
    std::string fn = itr->path().filename();
    size_t first = fn.find(".");
    size_t second = fn.find(".", first + 1);
    if (second != std::string::npos) {
      long x = strtol(&(fn.c_str()[first + 1]), NULL, 36);
      if (errno == ERANGE) {
        throw std::runtime_error("World is too big!");
      }
      long z = strtol(&(fn.c_str()[second + 1]), NULL, 36);
      if (errno == ERANGE) {
        throw std::runtime_error("World is too big!");
      }
      xPos_min_ = std::min(static_cast<int32_t>(x), xPos_min_);
      xPos_max_ = std::max(static_cast<int32_t>(x), xPos_max_);
      zPos_min_ = std::min(static_cast<int32_t>(z), zPos_min_);
      zPos_max_ = std::max(static_cast<int32_t>(z), zPos_max_);
    }
  }
  std::cout << "x: " << xPos_min_ << " " << xPos_max_ << std::endl;
  std::cout << "z: " << zPos_min_ << " " << zPos_max_ << std::endl;
}

bool nbt::exists(int32_t x, int32_t z, bf::path& path) const {
  int32_t x_tmp = x, z_tmp = z;
  while (x_tmp < 0) x_tmp += 64;
  while (z_tmp < 0) z_tmp += 64;
  path = dir_ / itoa(x_tmp % 64, 36) / itoa(z_tmp % 64, 36);
  if (!bf::exists(path)) {
    return false;
  }
  std::stringstream ss;
  ss << "c." << ((x < 0) ? "-" : "") << itoa(abs(x), 36) << "."
             << ((z < 0) ? "-" : "") << itoa(abs(z), 36) << ".dat";
  if (!bf::exists(path /= ss.str())) {
    return false;
  }
  return true;
}

nbt::tag_ptr nbt::tag_at(int32_t x, int32_t z) const {
  bf::path tmp;
  if(!exists(x, z, tmp)) {
    return tag_ptr();
  }
  gzFile filein = gzopen(tmp.string().c_str(), "rb");
  if (!filein) {
    throw std::runtime_error("file could not be opened! " + tmp.string());
  }
  int buffer = gzgetc(filein);
  switch (buffer) {
    case -1:
      throw std::runtime_error("file read error!" + tmp.string());
      break;
    case 10:
      {
        tag_ptr ret(new tag::tag_<tag::compound>(&filein, true));
        if (gzclose(filein) != Z_OK) {
          throw std::runtime_error("could not close file! " + tmp.string());
        }
        return ret;
      }
      break;
    default:
      throw std::runtime_error("wrong file format!" + tmp.string());
      break;
  }
  throw std::runtime_error("can't reach");
}

void nbt::setSettings(Settings set__) {
  set_ = set__;
  return;
}

char nbt::getValue(const nbt::map& cache,
                 int32_t x, int32_t y, int32_t z, int32_t j, int32_t i) const {
  if (y < 0 || y >= 128) {
    return 0;
  }
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
  nbt::map::const_iterator it = cache.find(std::pair<int, int>(j, i));
  if (it != cache.end()) {
    return it->second[static_cast<size_t>(y + z * 128 + x * 128 * 16)];
  } else {
    return 0;
  }
}

bool nbt::allEmptyBehind(const nbt::map& cache, int32_t j, int32_t i) const {
  for (int n = 1; n <= 16; ++n) {
    if (set_.rotate == 0) {
      nbt::map::const_iterator it = cache.find(std::pair<int, int>(j, i - n));
      if (it != cache.end())
        return false;
    } else if (set_.rotate == 1) {
      nbt::map::const_iterator it = cache.find(std::pair<int, int>(j - n, i));
      if (it != cache.end())
        return false;
    } else if (set_.rotate == 2) {
      nbt::map::const_iterator it = cache.find(std::pair<int, int>(j, i + n));
      if (it != cache.end())
        return false;
    } else if (set_.rotate == 3) {
      nbt::map::const_iterator it = cache.find(std::pair<int, int>(j + n, i));
      if (it != cache.end())
        return false;
    }
  }
  // std::cout << i << " " << j << std::endl;
  return true;
}

Color nbt::checkReliefDiagonal(const nbt::map& cache, Color input,
                                int x, int y, int z, int j, int i) const {
  int xd = 0, zd = 0;
  Color color = input;
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
  if ((colors[getValue(cache, x - xd, y, z, j, i)].alphaF() <= 0
    || colors[getValue(cache, x, y, z - zd, j, i)].alphaF() <= 0)
   && colors[getValue(cache, x - xd, y, z - zd, j, i)].alphaF() <= 0) {
    lighter_amount += set_.relief_strength;
  }
  if ((colors[getValue(cache, x + xd, y, z, j, i)].alphaF() <= 0
    || colors[getValue(cache, x, y, z + zd, j, i)].alphaF() <= 0)
   && colors[getValue(cache, x + xd, y, z + zd, j, i)].alphaF() <= 0) {
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

Color nbt::checkReliefNormal(const nbt::map& cache, Color input,
                              int x, int y, int z, int j, int i) const {
  Color color = input;
  int lighter_amount = 0;
  if (set_.sun_direction % 4 == 2) {
    if ((colors[getValue(cache, x + 1, y, z - 1, j, i)].alphaF() <= 0
      || colors[getValue(cache, x - 1, y, z - 1, j, i)].alphaF() <= 0)
     && colors[getValue(cache, x, y, z - 1, j, i)].alphaF() <= 0) {
      if (set_.sun_direction == 2)
        lighter_amount -= set_.relief_strength;
      if (set_.sun_direction == 6)
        lighter_amount += set_.relief_strength;
    }
    if ((colors[getValue(cache, x + 1, y, z + 1, j, i)].alphaF() <= 0
      || colors[getValue(cache, x - 1, y, z + 1, j, i)].alphaF() <= 0)
     && colors[getValue(cache, x, y, z + 1, j, i)].alphaF() <= 0) {
      if (set_.sun_direction == 2)
        lighter_amount += set_.relief_strength;
      if (set_.sun_direction == 6)
        lighter_amount -= set_.relief_strength;
    }
  } else if (set_.sun_direction % 4 == 0) {
    if ((colors[getValue(cache, x - 1, y, z + 1, j, i)].alphaF() <= 0
      || colors[getValue(cache, x - 1, y, z - 1, j, i)].alphaF() <= 0)
     && colors[getValue(cache, x - 1, y, z, j, i)].alphaF() <= 0) {
      if (set_.sun_direction == 4)
        lighter_amount -= set_.relief_strength;
      if (set_.sun_direction == 0)
        lighter_amount += set_.relief_strength;
    }
    if ((colors[getValue(cache, x + 1, y, z - 1, j, i)].alphaF() <= 0
      || colors[getValue(cache, x + 1, y, z + 1, j, i)].alphaF() <= 0)
     && colors[getValue(cache, x + 1, y, z, j, i)].alphaF() <= 0) {
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

Color nbt::calculateShadow(const nbt::map& cache, Color input,
                            int x, int y, int z, int j, int i,
                            bool zigzag) const {
  Color color = input;
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
    int sun_direction = ((set_.sun_direction - ((set_.rotate + 3) % 4) * 2) + 8) % 8;
    int shadow_amount;
    if (set_.topview) {
      shadow_amount = 0;
    } else {
      shadow_amount = ((sun_direction == 3 || sun_direction == 4
                     || sun_direction == 5) ? 0 : 1)
                    * !zigzag * set_.relief_strength * 2;
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
           colors[blockid].alphaF() <= 0)
       || (sun_direction == 2)
       || (sun_direction == 6)
       || (sun_direction == 7 &&
           colors[blockid2].alphaF() <= 0)) {
        shadow_amount >>= 2;
      }
    }
    zigzag = true;
    for (int iii = 0; iii < set_.shadow_quality + 1; ++iii) {
      Color light(0, 0, 0, 0);
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
        char blknr = getValue(cache, x, y, z, j, i);
        if (noShadow.count(blknr) == 0) {
          light = Color::blend(colors[blknr], light);
          if (light.alphaF() >= 1) {
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

Color nbt::calculateRelief(const nbt::map& cache, Color input,
                            int x, int y, int z, int j, int i) const {
  Color color = input;
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

Color nbt::calculateMap(const nbt::map& cache, Color input,
                         int x, int y, int z, int j, int i, bool zigzag) const {
  Color color = input;
  if (set_.topview) {
    if (set_.heightmap) {
      if (set_.color) {
        double pi = boost::math::constants::pi<double>();
        color = Color(std::atan(((1.0 - y / 127.0) - 0.5) * 10) / pi + 0.5,
                      1.0, 1.0, 1.0);
      } else {
        color = Color(y, y, y, 255);
      }
    } else {
      int height_low_bound = y;
      while (colors[getValue(cache, x, height_low_bound--, z, j, i)]
                                                              .alphaF() < 1) {
        if (height_low_bound == -1) break;
      }
      if (set_.shadow_quality_ultra) {
        for (int h = height_low_bound; h < y; ++h) {
          char blknr = getValue(cache, x, h, z, j, i);
          color = Color::blend(calculateShadow(cache, colors[blknr], x, h, z, j, i),
                                                                         color);
        }
      } else {
        for (int h = height_low_bound; h < y; ++h) {
          char blknr = getValue(cache, x, h, z, j, i);
          color = Color::blend(colors[blknr], color);
        }
      }
      color = Color::blend(calculateShadow(cache, colors[getValue(cache, x, y, z, j, i)], x, y, z, j, i), color);
    }
  } else if (set_.oblique) {
    std::stack<Color> colorstack;
    int& dec = (set_.rotate % 2 == 0) ? z : x;
    int blocks_hit = 0;
    do {
      int32_t blockid = getValue(cache, x, y, z, j, i);
      if (zigzag) {
        intmapit it = upperHalf.find(blockid);
        if (it != upperHalf.end()) blockid = (*it).second;
      } else {
        intmapit it = lowerHalf.find(blockid);
        if (it != lowerHalf.end()) blockid = (*it).second;
      }
      if (set_.shadow_quality_ultra || blocks_hit <= set_.shadow_quality * 2) {
        colorstack.push(calculateShadow(cache, colors_oblique[blockid], x, y, z, j, i,
                                                                       zigzag));
        ++blocks_hit;
      } else {
        colorstack.push(colors_oblique[blockid]);
      }
      if (blockid == 0) blocks_hit = false;
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
      if (!colorstack.empty() && colorstack.top().alphaF() >= 1) break;
      if (dec == -1 || dec == 16) {
        std::stack<Color> colorstack_inner = colorstack;
        bool clear = true;
        while (!colorstack.empty()) {
          if (colorstack.top().alphaF() > 0) clear = false;
          colorstack.pop();
        }
        if (clear) return Color(0, 0, 0, 0);
        colorstack = colorstack_inner;
        if (allEmptyBehind(cache, j, i)) break;
      }
    } while (y >= 0);
    Color tmp(0, 0, 0, 0);
    while (!colorstack.empty()) {
      tmp = Color::blend(colorstack.top(), tmp);
      colorstack.pop();
    }
    color = Color::blend(color, tmp);
  }
  return color;
}

inline int clamp(int value) {
  return (value > 255) ? 255 : ((value < 0) ? 0 : value);
}

void nbt::projectCoords(int32_t& x, int32_t& y, int32_t& z,
                        int32_t xx, int32_t zz, int32_t& state) const {
  if (set_.topview) {
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
    y = 127;
  } else if (set_.oblique) {
    state = (zz > 15) ? false : true;
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
  } else if (set_.isometric) {
    
  }
}

int32_t nbt::goOneStepIntoScene(const nbt::map& cache,
                                int32_t& x, int32_t& y, int32_t& z,
                                int32_t j, int32_t i,
                                int32_t& state) const {
  if (set_.topview) {
    --y;
  } else if (set_.oblique) {
    int& dec = (set_.rotate % 2 == 0) ? z : x;
    if (state) {
      if (set_.rotate <= 1) {
        --dec;
      } else {
        ++dec;
      }
      state = false;
    } else {
      --y;
      state = true;
    }
  }
  return getValue(cache, x, y, z, j, i);
}

void de_premultiply(Image<Color>& img) {
  for (size_t i = 0; i < img.rows; ++i) {
    for (size_t j = 0; j < img.cols; ++j) {
      Color& pixel = img.at(i, j, 0);
      if (pixel.alphaF() <= 0.0) {
        continue;
      }
      if (pixel.alphaF() < 1.0) {
        for (size_t c = 0; c < 3; ++c) {
          pixel.c[c] /= pixel.alphaF();
        }
      }
    }
  }
}

Image<uint8_t> nbt::getImage(int32_t j, int32_t i, bool* result) const {
  Image<Color> myimg;
  if (set_.topview) {
    myimg = Image<Color>(16, 16, 1);
  } else if (set_.oblique) {
    myimg = Image<Color>(16 + 128, 16, 1);
  } else if (set_.isometric) {
    myimg = Image<Color>(32 + 256, 16, 1);
  } else {
    throw std::runtime_error("at least one of topview, oblique or isometic must be chosen!");
  }
  tag_ptr tag = tag_at(j, i);
  if (tag) {
    int16_t minval = std::numeric_limits<int16_t>::min();
    uint32_t a = static_cast<uint32_t>(j - minval) << 16;
    uint32_t b = static_cast<uint32_t>(i - minval);
    uint32_t h_mask = 0xFFFF0000;
    uint32_t l_mask = 0x0000FFFF;
    boost::mt19937 gen((a & h_mask) | (b & l_mask));
    boost::uniform_int<> dist(-1, 1);
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> >
                                                              dither(gen, dist);
    const tag::tag* comp(tag->sub("Level"));
    int32_t xPos = comp->sub("xPos")->pay_<int32_t>();
    int32_t zPos = comp->sub("zPos")->pay_<int32_t>();
    std::string block_light = comp->sub("BlockLight")->pay_<tag::byte_array>().p;
    std::string sky_light = comp->sub("SkyLight")->pay_<tag::byte_array>().p;
    if (zPos != i || xPos != j) {
      std::cerr << "wrong tag in getImage!" << std::endl;
    }
    nbt::map cache;
    cache_mutex_->lock();
    for (int jj = j + 7; jj >= j - 7; --jj) {
      for (int ii = i + 7; ii >= i - 7; --ii) {
        nbt::map::iterator it = blockcache_.find(std::pair<int, int>(jj, ii));
        if (it == blockcache_.end()) {
          tag_ptr newtag = tag_at(jj, ii);
          if (newtag) {
            const std::string& pl = newtag->sub("Level")->
                                       sub("Blocks")->pay_<tag::byte_array>().p;
            blockcache_.insert(nbt::map::value_type(std::pair<int, int>(jj, ii),
                                                    pl));
            cache.insert(nbt::map::value_type(std::pair<int, int>(jj, ii), pl));
          }
        } else {
          cache.insert(*it);
        }
      }
    }
    cache_mutex_->unlock();
    for (uint16_t zz = 0; zz < myimg.rows; ++zz) {
      for (uint16_t xx = 0; xx < myimg.cols; ++xx) {
        int32_t x, y, z, state = -1;
        projectCoords(x, y, z, xx, zz, state);
        /* at this point x, y and z are block coordinates */
        int32_t block_type = getValue(cache, x, y, z, j, i);
        while (block_type == 0) {
          block_type = goOneStepIntoScene(cache, x, y, z, j, i, state);
          if (y < 0 || x < 0 || x > 15 || z < 0 || z > 15) {
            goto endloop1;
          }
        }
        {
          Color& color = myimg.at(zz, xx, 0);
          color = calculateMap(cache, color, x, y, z, j, i, state);
          color = calculateRelief(cache, color, x, y, z, j, i);
          color = color.lighter((y - 64) / 2 + 96);
          size_t light_index_tmp =
                            static_cast<size_t>(y + 1 + z * 128 + x * 128 * 16);
          if (y != 127) ++light_index_tmp;
          size_t light_index = light_index_tmp / 2;
          bool light_remainder = light_index_tmp % 2;
          int light = block_light[light_index];
          light = light_remainder ? (light & 0xF0) >> 4 : light & 0x0F;
          color = color.darker(50 + (15-light) * 100);
          int random1 = dither();
          int random2 = dither();
          color.setRedF(color.redF() * (1 + (random1 + random2) / 510.0));
          color.setGreenF(color.greenF() * (1 + (random1 + random2) / 510.0));
          color.setBlueF(color.blueF() * (1 + (random1 + random2) / 510.0));
        }
        endloop1:;
      }
    }
    *result = true;
  } else {
    *result = false;
  }
  de_premultiply(myimg);
  Image<uint8_t> dithered = myimg.floyd_steinberg();
  return dithered;
}

void nbt::clearCache() const {
  cache_mutex_->lock();
  blockcache_.clear();
  cache_mutex_->unlock();
}

std::string nbt::string() const {
  return (tag_ != 0) ? tag_->str() : "";
}
