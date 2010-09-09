#include "./nbt.h"

#include <iostream>

int main(int ac, const char* av[]) {
  if (ac != 2) {
    std::cerr << "Usage: ./nbtparse <filename>" << std::endl;
    exit(1);
  }
  nbt bigfile(av[1]);
  std::cout << bigfile.string();
  return 0;
}
