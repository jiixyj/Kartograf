#ifndef SRC_NBT_COLORS_H_
#define SRC_NBT_COLORS_H_

#include <boost/assign.hpp>
#include <QtGui>
#include <map>

class Color {
 public:
  Color() : c(4, 0.0) {}
  Color(double red, double green, double blue, double alpha) : c(4) {
    c[0] = alpha;
    c[1] = blue;
    c[2] = green;
    c[3] = red;
  }
  double redF() const { return c[3]; }
  double greenF() const { return c[2]; }
  double blueF() const { return c[1]; }
  double alphaF() const { return c[0]; }
  void setRedF(double red) { c[3] = red; }
  void setGreenF(double green) { c[2] = green; }
  void setBlueF(double blue) { c[1] = blue; }
  void setAlphaF(double alpha) { c[0] = alpha; }

  void toHSV() {
    double max = *std::max_element(&(c[0]), &(c[0]) + 4);
    double min = *std::min_element(&(c[0]), &(c[0]) + 4);
    double h, s, v;
    if (max >= min && max <= min) {
      h = 0.0;
    } else if (max <= c[3]) {
      h = (0.0 + (c[2] - c[1]) / (max - min)) / 6.0;
    } else if (max <= c[2]) {
      h = (2.0 + (c[1] - c[3]) / (max - min)) / 6.0;
    } else if (max <= c[1]) {
      h = (4.0 + (c[3] - c[2]) / (max - min)) / 6.0;
    }
    if (h < 0.0) h += 1.0;
    if (max <= 0.0) {
      s = 0.0;
    } else {
      s = (max - min) / max;
    }
    v = max;
    c[3] = h;
    c[2] = s;
    c[1] = v;
  }
  void toRGB() {
    int hi = static_cast<int>(c[3] * 6.0);
    double f = c[3] * 6.0 - hi;
    double p = c[1] * (1.0 - c[2]);
    double q = c[1] * (1.0 - c[2] * f);
    double t = c[1] * (1.0 - c[2] * (1.0 - f));
    switch (hi) {
      case 0:
      case 6:
        c[3] = c[1];
        c[2] = t;
        c[1] = p;
        break;
      case 1:
        c[3] = q;
        c[2] = c[1];
        c[1] = p;
        break;
      case 2:
        c[3] = p;
        c[2] = c[1];
        c[1] = t;
        break;
      case 3:
        c[3] = p;
        c[2] = q;
        c[1] = c[1];
        break;
      case 4:
        c[3] = t;
        c[2] = p;
        c[1] = c[1];
        break;
      case 5:
        c[3] = c[1];
        c[2] = p;
        c[1] = q;
        break;
      default:
        break;
    }
  }

  std::vector<double> c;
};

QColor blend(const QColor& B, const QColor& A);

typedef std::map<int, int>::const_iterator intmapit;

static std::map<int, int> upperHalf = boost::assign::map_list_of
  (6, 0)   // sapling
  (37, 0)  // yellow flower
  (38, 0)  // red flower
  (39, 0)  // brown mushroom
  (40, 0)  // red mushroom
  (44, 0)  // single step
  (51, 0)  // fire
  (55, 0)  // redstone wire
  (59, 0)  // crops
  (60, 0)  // soil
  (63, 0)  // sign
  (64, 0)  // wooden door
  (69, 0)  // lever
  (70, 0)  // stone pressure plate
  (71, 0)  // iron door
  (72, 0)  // wooden pressure plate
  (85, 0)  // fence
;

static std::map<int, int> lowerHalf = boost::assign::map_list_of
  (2, 3)   // grass is dirt underneath
  (50, 0)  // torch
  (75, 0)  // redstone torch (off)
  (76, 0)  // redstone torch (on)
  (78, 3)  // dito for snow
;

static std::set<int> noShadow = boost::assign::list_of
  (37)
  (38)
  (39)
  (40)
  (50)
  (51)
  (55)
  (60)
  (69)
  (70)
  (72)
  (76)
  (79)
;

static std::set<int> emitLight = boost::assign::list_of
  (50)
  (76)
;

typedef std::map<int, QColor>::iterator colorit;
static std::map<int, QColor> colors = boost::assign::map_list_of
  (0, QColor(0, 0, 0, 0))
  (1, QColor(125, 125, 125, 255))  // OK
  (2, QColor(117, 176, 73, 255))   // OK
  (3, QColor(134, 96, 67, 255))    // OK
  (4, QColor(117, 117, 117, 255))  // OK
  (5, QColor(157, 128, 79, 255))   // OK
  (6, QColor(29, 47, 8, 108))      // ( 29, 47,  8,108)
  (7, QColor(84, 84, 84, 255))     // OK
  (8, QColor(12, 27, 75, 138))     // OK (42, 94, 255, 138)
  (9, QColor(12, 27, 75, 138))     // OK (42, 94, 255, 138)
  (10, QColor(245, 65, 0, 255))    // OK
  (11, QColor(245, 65, 0, 255))    // OK
  (12, QColor(218, 210, 158, 255)) // OK
  (13, QColor(136, 126, 126, 255)) // OK
  (14, QColor(143, 140, 125, 255)) // OK
  (15, QColor(136, 130, 127, 255)) // OK
  (16, QColor(115, 115, 115, 255)) // OK
  (17, QColor(102, 81, 50, 255))   // OK top: (155,125, 77,255)
  (18, QColor(36, 116, 25, 154))   // ( 36,116, 25,154)
  (19, QColor(182, 182, 57, 255))  // OK
  (20, QColor(60, 67, 68, 71))     // ( 60, 67, 68, 71)
  (35, QColor(222, 222, 222, 255)) // OK
  (37, QColor(13, 20, 0, 31))      // OK
  (38, QColor(16, 5, 2, 30))       // OK
  (39, QColor(14, 11,  9, 26))     // OK
  (40, QColor(26,  7,  8, 34))     // OK
  (41, QColor(232, 165, 46, 255))  // OK
  (42, QColor(191, 191, 191, 255)) // OK
  (43, QColor(167, 167, 167, 255)) // OK
  (44, QColor(159, 159, 159, 255)) // OK
  (45, QColor(156, 110, 97, 255))  // OK
  (46, QColor(170, 93, 71, 255))   // OK
  (47, QColor(108, 88, 58,255))    // OK
  (48, QColor(91, 108, 91,255))    // OK
  (49, QColor(20, 18, 30, 255))    // OK
  (50, QColor(200, 200, 0, 200))
  (51, QColor(216, 147, 27, 216))  // ( 88, 57,  9, 88) ( 64, 45,  9, 64)
  (52, QColor(17, 48, 69, 155))    // OK
  (53, QColor(157, 128, 79, 255))  // OK
  (54, QColor(123, 91, 41, 255))   // OK
  (55, QColor( 11,  0,  0, 54))    // OK
  (56, QColor(129, 140, 143, 255)) // OK
  (57, QColor( 35,164,150,255))    // OK
  (58, QColor(107, 71, 43,255))    // OK
  (59, QColor( 57, 67,  5,166))    // OK
  (60, QColor( 75, 41, 14,255))    // OK
  (61, QColor( 78, 78, 78,255))    // OK
  (62, QColor(125,102, 85,255))    // OK
  (63, QColor(111, 91, 54, 255))
  (64, QColor(136, 109, 67, 255))
  (65, QColor( 68, 54, 30,143))    // OK
  (66, QColor( 68, 61, 50,143))    // OK
  (67, QColor(117, 117, 117, 255)) // OK
  (71, QColor(191, 191, 191, 255))
  (73, QColor(133,107,107,255))    // OK
  (74, QColor(133,107,107,255))    // OK
  (75, QColor(  7,  5,  3, 20))    // OK
  (76, QColor(200, 0, 0, 200))
  (78, QColor(240,251,251,255))    // OK
  (79, QColor(49,67,99,159))       // OK (125,173,255,159)
  (80, QColor(240,251,251,255))    // OK
  (81, QColor( 12, 93, 22,231))    // OK
  (82, QColor(159,164,177,255))    // OK
  (83, QColor( 82,106, 56,140))    // OK
  (84, QColor(107, 73, 55,255))    // OK
  (85, QColor( 68, 54, 30,143))    // OK (?)
;

#endif  // SRC_NBT_COLORS_H_
