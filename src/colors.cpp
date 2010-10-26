#include "./colors.h"

#include <stdexcept>

Color::Color() : c(4, 0.0) {}
Color::Color(int _red, int _green, int _blue, int _alpha) : c(4) {
  c[0] = _blue / 255.0;
  c[1] = _green / 255.0;
  c[2] = _red / 255.0;
  c[3] = _alpha / 255.0;
}
Color::Color(double _red, double _green, double _blue, double _alpha)
        : c(4) {
  c[0] = _blue;
  c[1] = _green;
  c[2] = _red;
  c[3] = _alpha;
}
double Color::redF() const { return c[2]; }
double Color::greenF() const { return c[1]; }
double Color::blueF() const { return c[0]; }
double Color::alphaF() const { return c[3]; }
void Color::setRedF(double _red) { c[2] = double_clamp(_red); }
void Color::setGreenF(double _green) { c[1] = double_clamp(_green); }
void Color::setBlueF(double _blue) { c[0] = double_clamp(_blue); }
void Color::setAlphaF(double _alpha) { c[3] = double_clamp(_alpha); }

inline double Color::double_clamp(double val) {
  if (val < 0.0) return 0.0;
  else if (val > 1.0) return 1.0;
  else return val;
}
void Color::toHSV() {
  double max = *std::max_element(&(c[0]), &(c[0]) + 3);
  double min = *std::min_element(&(c[0]), &(c[0]) + 3);
  double h, s, v;
  if (max >= min && max <= min) {
    h = 0.0;
  } else if (max <= c[2]) {
    h = (0.0 + (c[1] - c[0]) / (max - min)) / 6.0;
  } else if (max <= c[1]) {
    h = (2.0 + (c[0] - c[2]) / (max - min)) / 6.0;
  } else if (max <= c[0]) {
    h = (4.0 + (c[2] - c[1]) / (max - min)) / 6.0;
  } else {
    throw std::runtime_error("Can't reach! toHSV");
  }
  if (h < 0.0) h += 1.0;
  if (max <= 0.0) {
    s = 0.0;
  } else {
    s = (max - min) / max;
  }
  v = max;
  c[2] = h;
  c[1] = s;
  c[0] = v;
}
void Color::toRGB() {
  int hi = static_cast<int>(c[2] * 6.0);
  double f = c[2] * 6.0 - hi;
  double p = c[0] * (1.0 - c[1]);
  double q = c[0] * (1.0 - c[1] * f);
  double t = c[0] * (1.0 - c[1] * (1.0 - f));
  switch (hi) {
    case 0:
    case 6:
      c[2] = c[0];
      c[1] = t;
      c[0] = p;
      break;
    case 1:
      c[2] = q;
      c[1] = c[0];
      c[0] = p;
      break;
    case 2:
      c[2] = p;
      c[1] = c[0];
      c[0] = t;
      break;
    case 3:
      c[2] = p;
      c[1] = q;
      c[0] = c[0];
      break;
    case 4:
      c[2] = t;
      c[1] = p;
      c[0] = c[0];
      break;
    case 5:
      c[2] = c[0];
      c[1] = p;
      c[0] = q;
      break;
    default:
      throw std::runtime_error("Can't reach! toRGB");
      break;
  }
}
Color Color::blend(const Color& B, const Color& A) {
  Color C;
  C.setRedF(B.redF() + (1.0 - B.alphaF()) * A.redF());
  C.setGreenF(B.greenF() + (1.0 - B.alphaF()) * A.greenF());
  C.setBlueF(B.blueF() + (1.0 - B.alphaF()) * A.blueF());
  C.setAlphaF(B.alphaF() + (1.0 - B.alphaF()) * A.alphaF());
  return C;
}
Color Color::darker(int amount) const {
  Color ret = *this;
  ret.toHSV();
  ret.c[0] /= amount / 100.0;
  ret.c[0] = double_clamp(ret.c[0]);
  ret.toRGB();
  return ret;
}
Color Color::lighter(int amount) const {
  Color ret = *this;
  ret.toHSV();
  ret.c[0] *= amount / 100.0;
  ret.c[0] = double_clamp(ret.c[0]);
  ret.toRGB();
  return ret;
}
