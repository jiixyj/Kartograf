/* See LICENSE file for copyright and license details. */
#include "./minecraft_world.h"

#include <limits>
#include <sstream>
#include <fstream>

#include "./png_read.h"

namespace bf = boost::filesystem;

MinecraftWorld::MinecraftWorld(int world)
          : x_pos_min_(std::numeric_limits<int32_t>::max()),
            z_pos_min_(std::numeric_limits<int32_t>::max()),
            x_pos_max_(std::numeric_limits<int32_t>::min()),
            z_pos_max_(std::numeric_limits<int32_t>::min()),
            valid_coordinates_(),
            dir_(),
            has_biome_data_(false),
            foliage_data_(),
            grass_data_(),
            biome_indices_() {
  bf::path dir = find_world_path(world);
  if (dir.empty()) {
    throw std::runtime_error("Minecraft world not found!");
  }
  dir_ = dir;
  init_world();
}

MinecraftWorld::MinecraftWorld(const std::string& dir)
          : x_pos_min_(std::numeric_limits<int32_t>::max()),
            z_pos_min_(std::numeric_limits<int32_t>::max()),
            x_pos_max_(std::numeric_limits<int32_t>::min()),
            z_pos_max_(std::numeric_limits<int32_t>::min()),
            valid_coordinates_(),
            dir_(),
            has_biome_data_(false),
            foliage_data_(),
            grass_data_(),
            biome_indices_() {
  if (bf::is_directory(bf::status(dir_ = dir))) {
    init_world();
    return;
  }
  throw std::runtime_error("string must be a valid directory!");
}

bf::path MinecraftWorld::find_world_path(int world) {
  bf::path dir;
  char* home_dir;
  if ((home_dir = getenv("HOME"))) {
  } else if ((home_dir = getenv("APPDATA"))) {
  } else {
    return dir;
  }
  std::stringstream ss;
  ss << "World" << world;
  if (bf::exists(dir = bf::path(home_dir) / ".minecraft/saves/" / ss.str())) {
    return dir;
  } else if (bf::exists(dir = bf::path(home_dir) / "Library/"
                          "Application Support/minecraft/saves/" / ss.str())) {
    return dir;
  } else {
    return bf::path();
  }
}

bool MinecraftWorld::exists_block(int x, int z) const {
  return valid_coordinates_.count(std::make_pair(x, z));
}

MinecraftWorld::tag_ptr MinecraftWorld::get_tag_at(int x, int z) const {
  bf::path tmp = get_path_of_block(x, z);
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

void MinecraftWorld::init_world() {
  bf::path biome_dir = dir_ / "EXTRACTEDBIOMES";
  if (bf::is_directory(biome_dir)) {
    has_biome_data_ = true;
    uint32_t width, height;
    png_bytep* _foliage_data = read_png_file((dir_ / "EXTRACTEDBIOMES" / "foliagecolor.png").string().c_str(), width, height);
    png_bytep* _grass_data = read_png_file((dir_ / "EXTRACTEDBIOMES" / "grasscolor.png").string().c_str(), width, height);
    for (size_t y = 0; y < height; y++) {
      std::copy(_foliage_data[y], _foliage_data[y] + width * 4, std::back_inserter(foliage_data_));
      std::copy(_grass_data[y], _grass_data[y] + width * 4, std::back_inserter(grass_data_));
      delete[] _foliage_data[y];
      delete[] _grass_data[y];
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
        biome_indices_[std::make_pair(x, z)] = indices;
      }
    }
  }
  bool chunk_found = false;
  if (!bf::exists(dir_ / "level.dat")) {
    throw std::runtime_error("Invalid World folder!");
  }
  bf::recursive_directory_iterator end_itr;
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
        x_pos_min_ = std::min(x, x_pos_min_);
        x_pos_max_ = std::max(x, x_pos_max_);
        z_pos_min_ = std::min(z, z_pos_min_);
        z_pos_max_ = std::max(z, z_pos_max_);
        valid_coordinates_.insert(std::make_pair(x, z));
        chunk_found = true;
      }
    }
  }
  if (!chunk_found) {
    throw std::runtime_error("Not a valid Minecraft world!");
  }
  std::cout << "x: " << x_pos_min_ << " " << x_pos_max_ << std::endl;
  std::cout << "z: " << z_pos_min_ << " " << z_pos_max_ << std::endl;
}

bf::path MinecraftWorld::get_path_of_block(int x, int z) const {
  bf::path path = dir_ / itoa(((x % 64) + 64) % 64, 36)
                       / itoa(((z % 64) + 64) % 64, 36);
  std::stringstream ss;
  ss << "c." << ((x < 0) ? "-" : "") << itoa(abs(x), 36) << "."
             << ((z < 0) ? "-" : "") << itoa(abs(z), 36) << ".dat";
  path /= ss.str();
  return path;
}

std::string MinecraftWorld::itoa(int value, int base) {
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
