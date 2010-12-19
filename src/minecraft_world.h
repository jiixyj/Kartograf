#ifndef _MINECRAFT_WORLD_H
#define _MINECRAFT_WORLD_H

#include <set>
#include <map>
#include <boost/filesystem.hpp>

#include "./tag.h"

class MinecraftWorld {
 public:
  explicit MinecraftWorld(int world);
  explicit MinecraftWorld(const std::string&);

  static boost::filesystem::path find_world_path(int world);

  int x_pos_min() const { return x_pos_min_; }
  int z_pos_min() const { return z_pos_min_; }
  int x_pos_max() const { return x_pos_max_; }
  int z_pos_max() const { return z_pos_max_; }
  bool has_biome_data() const { return has_biome_data_; }
  const std::vector<char>& foliage_data() const { return foliage_data_; }
  const std::vector<char>& grass_data() const { return grass_data_; }
  typedef std::map<std::pair<int, int>, std::vector<uint16_t> > BiomeIndicesMap;
  const BiomeIndicesMap& biome_indices() const { return biome_indices_; }

  bool exists_block(int x, int z) const;

  typedef boost::shared_ptr<const tag::tag> tag_ptr;
  tag_ptr get_tag_at(int x, int z) const;

 private:
  int x_pos_min_;
  int z_pos_min_;
  int x_pos_max_;
  int z_pos_max_;

  std::set<std::pair<int, int> > valid_coordinates_;

  boost::filesystem::path dir_;

  bool has_biome_data_;
  std::vector<char> foliage_data_;
  std::vector<char> grass_data_;
  BiomeIndicesMap biome_indices_;

  void init_world();
  boost::filesystem::path get_path_of_block(int x, int z) const;

  static std::string itoa(int value, int base);
};

#endif  // _MINECRAFT_WORLD_H
