#include <iostream>
#include <cstdlib>

#include "./nbt.h"

int main(int ac, char* av[]) {
  if (ac != 2) {
    std::cerr << "Usage: ./nbtparse [filename | world number]" << std::endl;
    return 1;
  }
  int world = atoi(av[1]);
  nbt* bf;
  if (world == 0) {
    bf = new nbt(av[1]);
  } else {
    bf = new nbt(world);
  }
  std::cout << bf->string();
  Settings set;
  set.topview = false;
  set.oblique = true;
  set.heightmap = false;
  set.color = false;
  set.shadow_strength = 60;
  set.shadow_quality = true;
  set.shadow_quality_ultra = true;
  set.relief_strength = 10;
  set.sun_direction = 1;
  set.rotate = 1;
  set.sun_direction = (set.sun_direction + ((set.rotate + 3) % 4) * 2) % 8;
  if (set.shadow_strength != 0) {
    set.shadow = true;
  } else {
    set.shadow = false;
  }
  if (set.relief_strength != 0) {
    set.relief = true;
  } else {
    set.relief = false;
  }
  bf->setSettings(set);
  return 0;
}
