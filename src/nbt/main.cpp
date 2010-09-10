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
    int32_t xPos = bf.global.front()->sub("Level")->sub("xPos")->pay_int();
    int32_t zPos = bf.global.front()->sub("Level")->sub("zPos")->pay_int();
    int64_t time = bf.global.front()->sub("Level")->sub("LastUpdate")->pay_long();
    std::cout << xPos << " " << zPos << " " << time << std::endl;
  } else {
    nbt bigfile(world);
    // std::cout << bigfile.string();
  }
  return 0;
}
