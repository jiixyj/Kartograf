#ifndef SRC_NBT_COLORS_H_
#define SRC_NBT_COLORS_H_

#include <boost/assign.hpp>
#include <map>

class Color {
 public:
  Color();
  Color(int _red, int _green, int _blue, int _alpha = 255);
  Color(double _red, double _green, double _blue, double _alpha = 1.0);
  double redF() const;
  double greenF() const;
  double blueF() const;
  double alphaF() const;
  void setRedF(double _red);
  void setGreenF(double _green);
  void setBlueF(double _blue);
  void setAlphaF(double _alpha);

  static inline double double_clamp(double val);
  void toHSV();
  void toRGB();
  Color darker(int amount) const;
  Color lighter(int amount) const;

  static Color blend(const Color& B, const Color& A);

  std::vector<double> c;
};

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
  (78, 0)  // dito for snow
  (85, 0)  // fence
;

static std::map<int, int> lowerHalf = boost::assign::map_list_of
  (2, 3)   // grass is dirt underneath
  (50, 0)  // torch
  (75, 0)  // redstone torch (off)
  (76, 0)  // redstone torch (on)
  (78, 0)  // dito for snow
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
  (78)
  (79)
  (91)
;

static std::set<int> emitLight = boost::assign::list_of
  (10)
  (11)
  (50)
  (51)
  (76)
  (91)
;

typedef std::map<int, Color>::iterator colorit;
static std::map<int, Color> colors = boost::assign::map_list_of
  (0, Color(0, 0, 0, 0))
  (1, Color(125, 125, 125, 255))  // OK
  (2, Color(117, 176, 73, 255))   // OK
  (3, Color(134, 96, 67, 255))    // OK
  (4, Color(117, 117, 117, 255))  // OK
  (5, Color(157, 128, 79, 255))   // OK
  (6, Color(29, 47, 8, 108))      // ( 29, 47,  8,108)
  (7, Color(84, 84, 84, 255))     // OK
  (8, Color(12, 27, 75, 138))     // OK (42, 94, 255, 138)
  (9, Color(12, 27, 75, 138))     // OK (42, 94, 255, 138)
  (10, Color(245, 65, 0, 255))    // OK
  (11, Color(245, 65, 0, 255))    // OK
  (12, Color(218, 210, 158, 255)) // OK
  (13, Color(136, 126, 126, 255)) // OK
  (14, Color(143, 140, 125, 255)) // OK
  (15, Color(136, 130, 127, 255)) // OK
  (16, Color(115, 115, 115, 255)) // OK
  (17, Color(102, 81, 50, 255))   // OK top: (155,125, 77,255)
  (18, Color(36, 116, 25, 154))   // ( 36,116, 25,154)
  (19, Color(182, 182, 57, 255))  // OK
  (20, Color(60, 67, 68, 71))     // ( 60, 67, 68, 71)
  (35, Color(222, 222, 222, 255)) // OK
  (37, Color(13, 20, 0, 31))      // OK
  (38, Color(16, 5, 2, 30))       // OK
  (39, Color(14, 11,  9, 26))     // OK
  (40, Color(26,  7,  8, 34))     // OK
  (41, Color(232, 165, 46, 255))  // OK
  (42, Color(191, 191, 191, 255)) // OK
  (43, Color(167, 167, 167, 255)) // OK
  (44, Color(159, 159, 159, 255)) // OK
  (45, Color(156, 110, 97, 255))  // OK
  (46, Color(170, 93, 71, 255))   // OK
  (47, Color(108, 88, 58,255))    // OK
  (48, Color(91, 108, 91,255))    // OK
  (49, Color(20, 18, 30, 255))    // OK
  (50, Color(200, 200, 0, 200))
  (51, Color(216, 147, 27, 216))  // ( 88, 57,  9, 88) ( 64, 45,  9, 64)
  (52, Color(17, 48, 69, 155))    // OK
  (53, Color(157, 128, 79, 255))  // OK
  (54, Color(123, 91, 41, 255))   // OK
  (55, Color( 11,  0,  0, 54))    // OK
  (56, Color(129, 140, 143, 255)) // OK
  (57, Color( 35,164,150,255))    // OK
  (58, Color(107, 71, 43,255))    // OK
  (59, Color( 57, 67,  5,166))    // OK
  (60, Color( 75, 41, 14,255))    // OK
  (61, Color( 78, 78, 78,255))    // OK
  (62, Color(125,102, 85,255))    // OK
  (63, Color(111, 91, 54, 255))
  (64, Color(136, 109, 67, 255))
  (65, Color( 68, 54, 30,143))    // OK
  (66, Color( 68, 61, 50,143))    // OK
  (67, Color(117, 117, 117, 255)) // OK
  (71, Color(191, 191, 191, 255))
  (73, Color(133,107,107,255))    // OK
  (74, Color(133,107,107,255))    // OK
  (75, Color(  7,  5,  3, 20))    // OK
  (76, Color(200, 0, 0, 200))
  (78, Color(240,251,251,255))    // OK
  (79, Color(49,67,99,159))       // OK (125,173,255,159)
  (80, Color(240,251,251,255))    // OK
  (81, Color( 12, 93, 22,231))    // OK
  (82, Color(159,164,177,255))    // OK
  (83, Color( 82,106, 56,140))    // OK
  (84, Color(107, 73, 55,255))    // OK
  (85, Color( 68, 54, 30,143))    // OK (?)
  (86, Color(203,126, 27, 27))    // OK Pumpkin
  (87, Color(118, 61, 59,255))    // OK
  (88, Color( 82, 62, 49,255))    // OK
  (89, Color(143,119, 71,255))    // OK
  (90, Color( 34,  5, 73, 96))    // OK (?) Portal
  (91, Color(203,126, 27, 27))    // OK Jack-O-Lantern
;

static std::map<int, Color> make_colors_oblique() {
  std::map<int, Color> colors_oblique;
  std::map<int, Color>::const_iterator it;
  for (it = colors.begin(); it != colors.end(); ++it) {
    Color col = it->second;
    if (col.alphaF() > 0) {
      double old_alpha = col.alphaF();
      double new_alpha;
      new_alpha = old_alpha * old_alpha;
      col.setRedF(col.redF() / old_alpha * new_alpha);
      col.setGreenF(col.greenF() / old_alpha * new_alpha);
      col.setBlueF(col.blueF() / old_alpha * new_alpha);
      col.setAlphaF(new_alpha);
    }
    colors_oblique.insert(std::map<int, Color>::value_type(it->first, col));
  }
  return colors_oblique;
}

static std::map<int, Color> colors_oblique = make_colors_oblique();

#endif  // SRC_NBT_COLORS_H_
