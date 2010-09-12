#include <iostream>
#include <QtGui>
#include <limits>

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
  } else {
    nbt bf(world);
    QImage img((bf.xPos_max() - bf.xPos_min() + 1) * 16,
               (bf.zPos_max() - bf.zPos_min() + 1) * 16,
               QImage::Format_ARGB32_Premultiplied);
    img.fill(0);
    for (int i = bf.zPos_min(); i <= bf.zPos_max(); ++i) {
      for (int j = bf.xPos_min(); j <= bf.xPos_max(); ++j) {
        const nbt::tag_ptr tag = bf.tag_at(j, i);
        if (tag) {
          nbt::tag_ptr comp(tag->sub("Level"));
          int32_t xPos = comp->sub("xPos")->pay_<int32_t>();
          int32_t zPos = comp->sub("zPos")->pay_<int32_t>();
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
          for (int32_t ii = zPos_img; ii < zPos_img + 16; ++ii) {
            for (int32_t jj = xPos_img; jj < xPos_img + 16; ++jj) {
              img.setPixel(static_cast<int32_t>(jj), static_cast<int32_t>(ii),
                           QColor(0, 0, 0, 255).rgba());
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
