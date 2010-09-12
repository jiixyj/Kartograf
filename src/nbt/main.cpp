#include <iostream>
#include <QtGui>
#include <limits>
#include <cmath>

#include "./nbt.h"
#include "./settings.h"
#include "./colors.h"

int main(int ac, const char* av[]) {
  if (ac != 2) {
    std::cerr << "Usage: ./nbtparse [filename | world number]" << std::endl;
    exit(1);
  }
  int world = atoi(av[1]);
  if (world == 0) {
    nbt bf(av[1]);
    std::cout << bf.string();
  } else {
    nbt bf(world);
    Settings set;
    set.topview = true;
    set.heightmap = false;
    set.color = false;
    QImage img((bf.xPos_max() - bf.xPos_min() + 1) * 16,
               (bf.zPos_max() - bf.zPos_min() + 1) * 16,
               QImage::Format_ARGB32_Premultiplied);
    img.fill(0);
          int min = 300, max = -10;
    for (int i = bf.zPos_min(); i <= bf.zPos_max(); ++i) {
      for (int j = bf.xPos_min(); j <= bf.xPos_max(); ++j) {
        const nbt::tag_ptr tag = bf.tag_at(j, i);
        if (tag) {
          nbt::tag_ptr comp(tag->sub("Level"));
          int32_t xPos = comp->sub("xPos")->pay_<int32_t>();
          int32_t zPos = comp->sub("zPos")->pay_<int32_t>();
          const std::string& heightMap = comp->sub("HeightMap")->
                                               pay_<tag::byte_array>().p;
          const std::string& blocks = comp->sub("Blocks")->
                                               pay_<tag::byte_array>().p;
          const std::string& skylight = comp->sub("SkyLight")->
                                               pay_<tag::byte_array>().p;
          uint64_t xtmp = (xPos - bf.xPos_min()) * 16;
          uint64_t ztmp = (zPos - bf.zPos_min()) * 16;
          int32_t max_int = std::numeric_limits<int32_t>::max();
          if (xtmp + 15 > static_cast<uint64_t>(max_int)
           || ztmp + 15 > static_cast<uint64_t>(max_int)) {
            std::cerr << "Map is too large for an image!" << std::endl;
            exit(1);
          }
          int32_t xPos_img = static_cast<int32_t>(xtmp);
          int32_t zPos_img = static_cast<int32_t>(ztmp);
          int index = 0;
          for (int32_t ii = zPos_img; ii < zPos_img + 16; ++ii) {
            for (int32_t jj = xPos_img; jj < xPos_img + 16; ++jj) {
              uint8_t height = heightMap[index++];
              QColor color;
              if (set.heightmap) {
                if (set.color) {
                  color.setHsvF(atan(((1.0 - height / 127.0) - 0.5) * 10) / M_PI + 0.5, 1.0, 1.0, 1.0);
                } else {
                  color.setRgba(QColor(height, height, height, 255).rgba());
                }
              } else {
                int32_t ii0 = ii - zPos_img;
                int32_t jj0 = jj - xPos_img;
                for (int h = 0; h <= height; ++h) {
                  uint8_t blknr = blocks[h + ii0 * 128 + jj0 * 128 * 16];
                  color = blend(colors[blknr], color);
                }
                img.setPixel(static_cast<int32_t>(jj), static_cast<int32_t>(ii),
                             color.rgba());
              }
            }
          }
          // std::cout << j << " " << xPos << "  "
          //           << i << " " << zPos << std::endl;
        }
      }
    }
    img.save("test.png");
  }
  return 0;
}
