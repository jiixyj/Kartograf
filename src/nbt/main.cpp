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
    tag::tag* global = bf.global.front().get();
    int32_t xPos, zPos;
    xPos = global->sub("Level")->sub("xPos")->pay_<int32_t>();
    zPos = global->sub("Level")->sub("zPos")->pay_<int32_t>();
    std::cout << xPos << " " << zPos << " " << std::endl;
  } else {
    nbt bigfile(world);
    std::cout << bigfile.string();
  }
  return 0;
}
