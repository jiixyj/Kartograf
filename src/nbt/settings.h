#ifndef SRC_NBT_SETTINGS_H_
#define SRC_NBT_SETTINGS_H_

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

struct  Settings {
  bool topview;
    bool heightmap;
      bool color;
    bool relief;
    int relief_strength;
  bool oblique;
  bool shadow;
  bool shadow_quality;
  int shadow_strength;
  /* 0 = N, 1 = NW, 2 = W, 3 = SW, 4 = S, 5 = SE, 6 = E, 7 = NE */
  int sun_direction;
  int rotate;
  bool dither;
};

#endif  // SRC_NBT_SETTINGS_H_
