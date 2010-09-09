#include "./Color.h"

Color::Color() : QColor() {}

Color::Color(int r, int g, int b, int a) : QColor(r, g, b, a) {
  setRedF(redF() * alphaF());
  setGreenF(greenF() * alphaF());
  setBlueF(blueF() * alphaF());
}

Color::~Color() {}
