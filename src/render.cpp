#include "./render.h"

#include <iostream>

#include "./Color.h"

render::render() {
  Q = QImage(16, 16, QImage::Format_ARGB32_Premultiplied);
  Q.fill(256 + 256 << 8 + 256 << 16);
}

render::~render() {
}

render::render(int bb) {
  if (bb == 69) {
    Q = QImage(16, 144, QImage::Format_ARGB32_Premultiplied);

  } else if (bb == 70) {
    Q = QImage(33, 160, QImage::Format_ARGB32_Premultiplied);

  } else {
    Q = QImage(16, 16, QImage::Format_ARGB32_Premultiplied);
  }
  Q.fill(Color(255, 255, 255, 0).rgba());

  x = 0;
  y = 0;

}
