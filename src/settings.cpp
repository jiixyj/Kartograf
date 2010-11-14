#include "./settings.h"

Settings make_valid(Settings set_) {
  Settings set(set_);
  set.sun_direction = (set.sun_direction + set.isometric + ((set.rotate + 3) % 4) * 2) % 8;
  set.shadow = set.shadow_strength;
  set.relief = set.relief_strength;
  return set;
}
