#include <iostream>

#include "./nbt.h"

int main(int ac, const char* av[]) {
  if (ac != 2) {
    std::cerr << "Usage: ./nbtparse [filename | world number]" << std::endl;
    exit(1);
  }
  int world = atoi(av[1]);
  if (world == 0) {
    nbt bf(av[1]);
    std::cout << bf.string();
  } else {
    nbt bf(world);
    for (int i = bf.zPos_min(); i <= bf.zPos_max(); ++i) {
      for (int j = bf.xPos_min(); j <= bf.xPos_max(); ++j) {
        const nbt::tag_ptr tag = bf.tag_at(j, i);
        if (tag) {
          nbt::tag_ptr comp(tag->sub("Level"));
          int32_t xPos = comp->sub("xPos")->pay_<int32_t>();
          int32_t zPos = comp->sub("zPos")->pay_<int32_t>();
          std::cout << j << " " << xPos << "  "
                    << i << " " << zPos << std::endl;
        }
      }
    }
  }
  return 0;
}
