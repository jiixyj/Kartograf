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
  } else {
    nbt bigfile(world);
    std::cout << bigfile.string();
  }
  return 0;
}
