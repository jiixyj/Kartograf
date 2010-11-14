#ifndef SRC_NBT_SETTINGS_H_
#define SRC_NBT_SETTINGS_H_

struct  Settings {
  Settings() : topview(true),
               heightmap(false),
               color(false),
               relief(false),
               relief_strength(0),
               oblique(false),
               isometric(false),
               shadow(false),
               shadow_quality(false),
               shadow_quality_ultra(false),
               shadow_strength(0),
               nightmode(false),
               sun_direction(1),
               rotate(1) {
  }
  bool topview;
    bool heightmap;
      bool color;
    bool relief;
    int relief_strength;
  bool oblique;
  bool isometric;
  bool shadow;
  bool shadow_quality;
  bool shadow_quality_ultra;
  int shadow_strength;
  bool nightmode;
  /* 0 = N, 1 = NW, 2 = W, 3 = SW, 4 = S, 5 = SE, 6 = E, 7 = NE */
  int sun_direction;
  int rotate;
};

Settings make_valid(Settings set_);

#endif  // SRC_NBT_SETTINGS_H_
