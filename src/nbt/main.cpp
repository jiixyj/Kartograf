#include <iostream>

#include "./nbt.h"

int main(int ac, const char* av[]) {
  if (ac != 2) {
    std::cerr << "Usage: ./nbtparse [filename | world number]" << std::endl;
    exit(1);
  }
  int world = atoi(av[1]);
  if (world == 0) {
    nbt bigfile(av[1]);
    std::cout << bigfile.string();
    int32_t xPos = bigfile.global.front()->sub("Level")
                                                    ->sub(3, "xPos")->pay_int();
    int32_t zPos = bigfile.global.front()->sub("Level")
                                                    ->sub(3, "zPos")->pay_int();
    std::cout << xPos << " " << zPos << std::endl;
  } else {
    nbt bigfile(world);
    // std::cout << bigfile.string();
  }
  return 0;
}
