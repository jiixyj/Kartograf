/* See LICENSE file for copyright and license details. */
#include "./nbt.h"

#include <boost/math/constants/constants.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <fstream>

#include "./png_read.h"

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

nbt::nbt(): bad_world(false),
            tag_(),
            xPos_min_(std::numeric_limits<int32_t>::max()),
            zPos_min_(std::numeric_limits<int32_t>::max()),
            xPos_max_(std::numeric_limits<int32_t>::min()),
            zPos_max_(std::numeric_limits<int32_t>::min()),
            dir_(getenv("HOME")),
            set_(),
            has_biome_data(false),
            foliage_data(),
            grass_data(),
            biome_indices(),
            cache_mutex_(new boost::mutex),
            blockcache_() {}

nbt::nbt(int world)
          : bad_world(false),
            tag_(),
            xPos_min_(std::numeric_limits<int32_t>::max()),
            zPos_min_(std::numeric_limits<int32_t>::max()),
            xPos_max_(std::numeric_limits<int32_t>::min()),
            zPos_max_(std::numeric_limits<int32_t>::min()),
            dir_(),
            set_(),
            has_biome_data(false),
            foliage_data(),
            grass_data(),
            biome_indices(),
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
          : bad_world(false),
            tag_(),
            xPos_min_(std::numeric_limits<int32_t>::max()),
            zPos_min_(std::numeric_limits<int32_t>::max()),
            xPos_max_(std::numeric_limits<int32_t>::min()),
            zPos_max_(std::numeric_limits<int32_t>::min()),
            dir_(),
            set_(),
            has_biome_data(false),
            foliage_data(),
            grass_data(),
            biome_indices(),
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

bool nbt::exist_world(int world) {
  boost::filesystem::path dir;
  char* home_dir;
  if ((home_dir = getenv("HOME"))) {
  } else if ((home_dir = getenv("APPDATA"))) {
  } else {
    return false;
  }
  std::stringstream ss;
  ss << "World" << world;
  if (bf::exists(dir = bf::path(home_dir) / ".minecraft/saves/" / ss.str())) {
    return true;
  } else if (bf::exists(dir = bf::path(home_dir) / "Library/"
                          "Application Support/minecraft/saves/" / ss.str())) {
    return true;
  } else {
    return false;
  }
}

void nbt::construct_world() {
  bf::path biome_dir = dir_ / "EXTRACTEDBIOMES";
  if (bf::is_directory(biome_dir)) {
    has_biome_data = true;
    uint32_t width, height;
    png_bytep* foliage_data_ = read_png_file((dir_ / "EXTRACTEDBIOMES" / "foliagecolor.png").string().c_str(), width, height);
    png_bytep* grass_data_ = read_png_file((dir_ / "EXTRACTEDBIOMES" / "grasscolor.png").string().c_str(), width, height);
    for (size_t y = 0; y < height; y++) {
      std::copy(foliage_data_[y], foliage_data_[y] + width * 4, std::back_inserter(foliage_data));
      std::copy(grass_data_[y], grass_data_[y] + width * 4, std::back_inserter(grass_data));
      delete[] foliage_data_[y];
      delete[] grass_data_[y];
    }
    bf::recursive_directory_iterator end_biome_itr;
    for (bf::recursive_directory_iterator itr(biome_dir); itr != end_biome_itr; ++itr) {
      if (bf::extension(itr->path()).compare(".biome")) continue;
      std::string fn = itr->path().filename();
      size_t first = fn.find(".");
      size_t second = fn.find(".", first + 1);
      if (second != std::string::npos) {
        long x = strtol(&(fn.c_str()[0]), NULL, 10);
        long z = strtol(&(fn.c_str()[first + 1]), NULL, 10);
        std::cout << x << " " << z << std::endl;
        std::ifstream input(itr->path().string().c_str());
        std::vector<uint16_t> indices;
        for(int i = 0; i < 256*64; ++i) {
          uint16_t dummy;
          input.read(reinterpret_cast<char*>(&dummy) + 1, 1);
          input.read(reinterpret_cast<char*>(&dummy), 1);
          indices.push_back(dummy);
        }
        biome_indices[std::make_pair(x, z)] = indices;
      }
    }
  }
  bf::recursive_directory_iterator end_itr;
  bool chunk_found = false;
  if (!bf::exists(dir_ / "level.dat")) {
    throw std::runtime_error("Invalid World folder!");
  }
  for (bf::recursive_directory_iterator itr(dir_); itr != end_itr; ++itr) {
    if (bf::is_directory(itr->path())) continue;
    if (bf::extension(itr->path()).compare(".dat")) continue;
    std::string fn = itr->path().filename();
    size_t first = fn.find(".");
    size_t second = fn.find(".", first + 1);
    if (second != std::string::npos) {
      int x = static_cast<int>(strtol(&(fn.c_str()[first + 1]), NULL, 36));
      int z = static_cast<int>(strtol(&(fn.c_str()[second + 1]), NULL, 36));
      bf::path check = dir_ / itoa(((x % 64) + 64) % 64, 36)
                            / itoa(((z % 64) + 64) % 64, 36) / fn;
      if (bf::exists(check)) {
        xPos_min_ = std::min(x, xPos_min_);
        xPos_max_ = std::max(x, xPos_max_);
        zPos_min_ = std::min(z, zPos_min_);
        zPos_max_ = std::max(z, zPos_max_);
        chunk_found = true;
      }
    }
  }
  bad_world = !chunk_found;
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

// std::list<point3> a_star(int x_start, int z_start,
//                          int x_end, int z_end) {
//         // nbt::map::iterator it = blockcache_.find(std::pair<int, int>(jj, ii));
//         // if (it == blockcache_.end()) {
//         //   tag_ptr newtag = tag_at(jj, ii);
//         //   if (newtag) {
//         //     const std::string& pl = newtag->sub("Level")->
//         //                                sub("Blocks")->pay_<tag::byte_array>().p;
//         //     blockcache_.insert(nbt::map::value_type(std::pair<int, int>(jj, ii),
//         //                                             pl));
//         //     cache.insert(nbt::map::value_type(std::pair<int, int>(jj, ii), pl));
//         //   }
//         // } else {
//         //   cache.insert(*it);
//         // }
//         // int32_t block_type = getValue(cache, x, y, z, j, i);
//         // changeBlockParts(block_type, state);
//         // while (block_type == 0) {
//         //   old_x = x; old_y = y; old_z = z;
//         //   goOneStepIntoScene(x, y, z, state);
//         //   if (y < 0 || x < 0 || x > 15 || z < 0 || z > 15) {
//         //     goto endloop1;
//         //   }
//         //   block_type = getValue(cache, x, y, z, j, i);
//         //   changeBlockParts(block_type, state);
//         // }
// }

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
  set_ = make_valid(set__);
  return;
}

char nbt::getValue(const nbt::map& cache,
                 int32_t x, int32_t y, int32_t z, int32_t j, int32_t i) const {
  if (y < 0 || y >= 128) {
    std::cerr << "this is probably a bug" << std::endl;
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
                            int32_t zigzag) const {
  Color color = input;
  if (set_.shadow) {
    int blockid = getValue(cache, x, y, z, j, i);
    changeBlockParts(blockid, zigzag);
    if (emitLight.count(blockid) == 1) {
      return color;
    }
    ++y;
    int yy = y;
    int xx = x;
    int zz = z;
    int sun_direction = ((set_.sun_direction - ((set_.rotate + 3) % 4) * 2) + 8) % 8;
    int shadow_amount = 0;
    if (set_.topview) {
      shadow_amount = 0;
    } else {
      if (set_.oblique) {
        if (!zigzag)
          shadow_amount = !(sun_direction == 3 || sun_direction == 4
                         || sun_direction == 5)
                        * set_.shadow_strength / 3;
      } else if (set_.isometric) {
        if (zigzag == 4 || zigzag == 8 || zigzag == 9 || zigzag == 13)
          shadow_amount = !(sun_direction == 3 || sun_direction == 4
                         || sun_direction == 5)
                        * set_.shadow_strength / 3;
        else if (zigzag == 7 || zigzag == 10 || zigzag == 11 || zigzag == 14)
          shadow_amount = !(sun_direction == 6 || sun_direction == 7
                          || sun_direction == 8)
                        * set_.shadow_strength / 3;
      }
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
       || (sun_direction == 2 && (!set_.isometric || zigzag == 4 || zigzag == 8 || zigzag == 9 || zigzag == 13))
       || (sun_direction == 6 && (!set_.isometric || zigzag == 7 || zigzag == 10 || zigzag == 11 || zigzag == 14))
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

void nbt::changeBlockParts(int32_t& blockid, int state) const {
  if (set_.oblique) {
    if (state) {
      intmapit it = upperHalf.find(blockid);
      if (it != upperHalf.end()) blockid = (*it).second;
    } else {
      intmapit it = lowerHalf.find(blockid);
      if (it != lowerHalf.end()) blockid = (*it).second;
    }
  } else if (set_.isometric) {
    if (state == 1 || state == 2 || state == 5 || state == 6) {
      intmapit it = upperHalf.find(blockid);
      if (it != upperHalf.end()) blockid = (*it).second;
    } else {
      if ((blockid == 2) && (state == 4 || state == 9 || state == 10 || state == 7)) return;
      intmapit it = lowerHalf.find(blockid);
      if (it != lowerHalf.end()) blockid = (*it).second;
    }
  }
}

Color nbt::blockid_to_color(int value, int x, int z, int j, int i,
                            bool oblique) const {
  if (has_biome_data && (value == 2 || value == 18)) {
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
    int i_diff = ((i % 8) + 8) % 8;
    int j_diff = ((j % 8) + 8) % 8;
    int i_eight = i - i_diff;
    int j_eight = j - j_diff;
    const std::vector<uint16_t>& data = biome_indices.at(std::make_pair(j_eight, i_eight));
    if (data.size() != 16384) {
      throw std::runtime_error("Error opening biome indices!");
    }
    uint16_t image_index = data.at(static_cast<size_t>(
                                   i_diff * 256 * 8
                                 + z * 16 * 8
                                 + j_diff * 16
                                 + x));

    if (value == 2) {
      return Color(static_cast<uint8_t>(grass_data[image_index * 4u + 0]),
                   static_cast<uint8_t>(grass_data[image_index * 4u + 1]),
                   static_cast<uint8_t>(grass_data[image_index * 4u + 2]),
                   255);
    } else if (value == 18) {
      Color ret (static_cast<uint8_t>(foliage_data[image_index * 4u + 0]),
                 static_cast<uint8_t>(foliage_data[image_index * 4u + 1]),
                 static_cast<uint8_t>(foliage_data[image_index * 4u + 2]),
                 255);
      Color leaves = oblique ? colors_oblique[value] : colors[value];
      ret.setAlphaF(leaves.alphaF());
      ret.setRedF(leaves.alphaF() * ret.redF());
      ret.setGreenF(leaves.alphaF() * ret.greenF());
      ret.setBlueF(leaves.alphaF() * ret.blueF());
      return ret;
    }
  }
  return oblique ? colors_oblique[value] : colors[value];
}

Color nbt::calculateMap(const nbt::map& cache, Color input,
                         int x, int y, int z, int j, int i, int32_t zigzag) const {
  Color color = input;
  if (set_.topview) {
    if (set_.heightmap) {
      if (set_.color) {
        double pi = boost::math::constants::pi<double>();
        color = Color(std::atan(((1.0 - y / 127.0) - 0.5) * 10) / pi + 0.5,
                      1.0, 0.8, 1.0);
        color.toRGB();
      } else {
        color = Color(y, y, y, 255);
      }
    } else {
      int height_low_bound = y;
      while (blockid_to_color(getValue(cache, x, height_low_bound--, z, j, i), x, z, j, i)
                                                              .alphaF() < 1) {
        if (height_low_bound == -1) break;
      }
      if (set_.shadow_quality_ultra) {
        for (int h = height_low_bound; h < y; ++h) {
          char blknr = getValue(cache, x, h, z, j, i);
          if (blknr != 0) {
            color = Color::blend(calculateShadow(cache, blockid_to_color(blknr, x, z, j, i), x, h, z, j, i),
                                                                           color);
          }
        }
      } else {
        for (int h = height_low_bound; h < y; ++h) {
          char blknr = getValue(cache, x, h, z, j, i);
          color = Color::blend(blockid_to_color(blknr, x, z, j, i), color);
        }
      }
      color = Color::blend(calculateShadow(cache, blockid_to_color(getValue(cache, x, y, z, j, i), x, z, j, i), x, y, z, j, i), color);
    }
  } else if (set_.oblique || set_.isometric) {
    std::stack<Color> colorstack;
    int blocks_hit = 0;
    int32_t blockid;
    while (y >= 0) {
      blockid = getValue(cache, x, y, z, j, i);
      changeBlockParts(blockid, zigzag);
      if (y != 127 && getValue(cache, x, y + 1, z, j, i) == 78) {
        if (set_.isometric && (zigzag == 8 || zigzag == 13 || zigzag == 14 || zigzag == 11)) ;
        else if (set_.oblique && zigzag == 0) ;
        else
          blockid = 78;
      }
      size_t color_map_size = colors_oblique.size();
      if (set_.shadow_quality_ultra || blocks_hit <= set_.shadow_quality * 2) {
        if (blockid != 0) {
          colorstack.push(calculateShadow(cache, blockid_to_color(blockid, x, z, j, i, true), x, y, z, j, i,
                                                                         zigzag));
        }
        ++blocks_hit;
      } else {
        colorstack.push(blockid_to_color(blockid, x, z, j, i, true));
      }
      if (colors_oblique.size() > color_map_size) {
        std::cerr << "Block " << blockid << "has not been implemented yet\n";
      }
      if (blockid == 0) blocks_hit = false;
      goOneStepIntoScene(x, y, z, zigzag);
      if (!colorstack.empty() && colorstack.top().alphaF() >= 1) break;
      if (x == -1 || z == -1 || x == 16 || z == 16) {
        std::stack<Color> colorstack_inner = colorstack;
        bool clear = true;
        while (!colorstack.empty()) {
          if (colorstack.top().alphaF() > 0) clear = false;
          colorstack.pop();
        }
        if (clear) return Color(0, 0, 0, 0);
        colorstack = colorstack_inner;
      }
    }
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

static std::vector<std::vector<int> > make_state_mask() {
  const int max_y = 127;
  const int half_x = 32;
  std::vector<std::vector<int> > ret(288, std::vector<int>(64, -1));
  for (int y = 0; y < 128; ++y) {
    for (int z = 15; z >= 0; --z) {
      for (int x = 15; x >= 0; --x) {
        int px = 15-x;
        int pz = 15-z;
        int cx, cy;
        cx = 2 * px - 2 * pz;
        cy = px + 2 * (max_y-y) + pz;
        cy += 2;
        cx += half_x;
        int index = 0;
        for (int i = -2; i < 2; ++i) {
          for (int j = -2; j < 2; ++j) {
            ret[static_cast<size_t>(cy+i)][static_cast<size_t>(cx+j)] = index++;
          }
        }
      }
    }
  }
  return ret;
}

static std::vector<std::vector<std::vector<int > > > make_to3D() {
  const int max_y = 127;
  const int half_x = 32;
  std::vector<std::vector<std::vector<int > > > to3D(288, std::vector<std::vector<int> >(64, std::vector<int>(3,-1)));
  for (int y = 0; y < 128; ++y) {
    for (int z = 15; z >= 0; --z) {
      for (int x = 15; x >= 0; --x) {
        int px = 15-x;
        int pz = 15-z;
        int cx, cy;
        cx = 2 * px - 2 * pz;
        cy = px + 2 * (max_y-y) + pz;
        cy += 2;
        cx += half_x;
        for (int i = -2; i < 2; ++i) {
          for (int j = -2; j < 2; ++j) {
            size_t index_y = static_cast<size_t>(cy + i);
            size_t index_x = static_cast<size_t>(cx + j);
            to3D[index_y][index_x][0] = x;
            to3D[index_y][index_x][1] = y;
            to3D[index_y][index_x][2] = z;
          }
        }
      }
    }
  }
  return to3D;
}

static std::vector<std::vector<int> > state_mask = make_state_mask();
static std::vector<std::vector<std::vector<int > > > to3D = make_to3D();

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
    state = 0;
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
    size_t index_zz = static_cast<size_t>(zz);
    size_t index_xx = static_cast<size_t>(xx);
    state = state_mask[index_zz][index_xx];
    x = to3D[index_zz][index_xx][0];
    y = to3D[index_zz][index_xx][1];
    z = to3D[index_zz][index_xx][2];
    if (set_.rotate == 3) {
      int tmp = x;
      x = z;
      z = 15 - tmp;
    } else if (set_.rotate == 0) {
      x = 15 - x;
      z = 15 - z;
    } else if (set_.rotate == 1) {
      int tmp = x;
      x = 15 - z;
      z = tmp;
    }
    if (state == 0 || state == 3 || state == 12 || state == 15) {
      goOneStepIntoScene(x, y, z, state);
      if (y < 0 || x < 0 || x > 15 || z < 0 || z > 15) {
        state = -1;
      }
    }
  }
}

void nbt::goOneStepIntoScene(int32_t& x, int32_t& y, int32_t& z,
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
  } else if (set_.isometric) {
    if (set_.rotate == 3) {
      int tmp = z;
      z = x;
      x = 15 - tmp;
    } else if (set_.rotate == 0) {
      x = 15 - x;
      z = 15 - z;
    } else if (set_.rotate == 1) {
      int tmp = z;
      z = 15 - x;
      x = tmp;
    }
    switch (state) {
      case 0:
        state = 6;
        x = x + 1;
        break;
      case 1:
        state = 7;
        x = x + 1;
        break;
      case 2:
        state = 4;
        z = z + 1;
        break;
      case 3:
        state = 5;
        z = z + 1;
        break;
      case 4:
        state = 10;
        x = x + 1;
        break;
      case 5:
        state = 16;
        break;
      case 6:
        state = 17;
        break;
      case 7:
        state = 9;
        z = z + 1;
        break;
      case 8:
        state = 14;
        x = x + 1;
        break;
      case 9:
        state = 18;
        break;
      case 10:
        state = 19;
        break;
      case 11:
        state = 13;
        z = z + 1;
        break;
      case 12:
        state = 4;
        y = y - 1;
        break;
      case 13:
        state = 5;
        y = y - 1;
        break;
      case 14:
        state = 6;
        y = y - 1;
        break;
      case 15:
        state = 7;
        y = y - 1;
        break;
      case 16:
        state = 11;
        x = x + 1;
        break;
      case 17:
        state = 8;
        z = z + 1;
        break;
      case 18:
        state = 1;
        y = y - 1;
        break;
      case 19:
        state = 2;
        y = y - 1;
        break;
      default:
        throw std::runtime_error("Does not happen!");
        break;
    }
    if (set_.rotate == 3) {
      int tmp = x;
      x = z;
      z = 15 - tmp;
    } else if (set_.rotate == 0) {
      x = 15 - x;
      z = 15 - z;
    } else if (set_.rotate == 1) {
      int tmp = x;
      x = 15 - z;
      z = tmp;
    }
  }
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
    myimg = Image<Color>(32 + 256, 64, 1);
  } else {
    throw std::runtime_error("at least one of topview, oblique or isometric must be chosen!");
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
        int32_t old_x = x, old_y = y, old_z = z;
        if (state == -1) continue;
        /* at this point x, y and z are block coordinates */
        int32_t block_type = getValue(cache, x, y, z, j, i);
        changeBlockParts(block_type, state);
        while (block_type == 0) {
          old_x = x; old_y = y; old_z = z;
          goOneStepIntoScene(x, y, z, state);
          if (y < 0 || x < 0 || x > 15 || z < 0 || z > 15) {
            goto endloop1;
          }
          block_type = getValue(cache, x, y, z, j, i);
          changeBlockParts(block_type, state);
        }
        {
          Color& color = myimg.at(zz, xx, 0);
          color = calculateMap(cache, color, x, y, z, j, i, state);
          color = calculateRelief(cache, color, x, y, z, j, i);
          if (!set_.heightmap) {
            if (set_.nightmode) {
              size_t light_index_tmp =
                                static_cast<size_t>(old_y + old_z * 128 + old_x * 128 * 16);
              size_t light_index = light_index_tmp / 2;
              bool light_remainder = light_index_tmp % 2;
              int light = block_light[light_index];
              light = light_remainder ? (light & 0xF0) >> 4 : light & 0x0F;
              light = std::max(4, light);
              if (!emitLight.count(block_type))
                color = color.lighter(static_cast<int>(100 * std::pow(0.8, (15-light)) + 0.5));
            }
          }
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
