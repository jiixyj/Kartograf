#include "./nbt.h"

#include <iostream>

int main() {
  nbt bigfile("bigtest.nbt");
  std::cout << bigfile.string();
  return 0;
}
