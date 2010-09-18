#ifndef SRC_NBT_COLORS_H_
#define SRC_NBT_COLORS_H_

#include <boost/assign.hpp>
#include <QtGui>
#include <map>

QColor blend(const QColor& B, const QColor& A) {
  QColor C;
  C.setRedF(B.redF() + (1 - B.alphaF()) * A.redF());
  C.setGreenF(B.greenF() + (1 - B.alphaF()) * A.greenF());
  C.setBlueF(B.blueF() + (1 - B.alphaF()) * A.blueF());
  C.setAlphaF(B.alphaF() + (1 - B.alphaF()) * A.alphaF());
  return C;
}

typedef std::map<int, int>::const_iterator intmapit;

static std::map<int, int> upperHalf = boost::assign::map_list_of
  (6, 0)   // sapling
  (37, 0)  // yellow flower
  (38, 0)  // red flower
  (39, 0)  // brown mushroom
  (40, 0)  // red mushroom
  (44, 0)  // single step
  (50, 0)  // torch
  (51, 0)  // fire
  (55, 0)  // redstone wire
  (59, 0)  // crops
  (60, 0)  // soil
  (63, 0)  // sign
  (69, 0)  // lever
  (70, 0)  // stone pressure plate
  (72, 0)  // wooden pressure plate
  (75, 0)  // redstone torch (off)
  (76, 0)  // redstone torch (on)
  (79, 0)  // ice
  (85, 0)  // fence
;

static std::map<int, int> lowerHalf = boost::assign::map_list_of
  (2, 3)   // grass is dirt underneath
  (78, 3)  // dito for snow
;

static std::set<int> noShadow = boost::assign::list_of
  (2)
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
