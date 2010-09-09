#include "./render.h"

#include <iostream>

render::render() {
  Q = QImage(16, 16, QImage::Format_ARGB32_Premultiplied);
  Q.fill(QColor(0, 0, 0, 0).rgba());
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
  Q.fill(QColor(0, 0, 0, 0).rgba());

  x = 0;
  y = 0;

}
