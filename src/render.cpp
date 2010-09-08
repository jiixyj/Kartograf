#include "./render.h"
#include <iostream>

render::render() {
  Q = QImage(16, 16, QImage::Format_ARGB32);
  Q.fill(256 + 256 << 8 + 256 << 16);
}

render::~render() {
}

render::render(int bb) {
  if (bb == 69) {
    Q = QImage(16, 144, QImage::Format_ARGB32);

  } else if (bb == 70) {
    Q = QImage(33, 160, QImage::Format_ARGB32);

  } else {
    Q = QImage(16, 16, QImage::Format_ARGB32);
  }
  Q.fill(QColor(255, 255, 255, 0).rgba());

  x = 0;
  y = 0;

}
