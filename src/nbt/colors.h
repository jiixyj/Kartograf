#include <boost/assign.hpp>
#include <QtGui>

QColor blend(QColor B, QColor A, int h) {
  QColor C;
  C.setRedF(B.redF() + (1 - B.alphaF()) * A.redF()*(h / 128.0));
  C.setGreenF(B.greenF() + (1 - B.alphaF()) * A.greenF()*(h / 128.0));
  C.setBlueF(B.blueF() + (1 - B.alphaF()) * A.blueF()*(h / 128.0));
  C.setAlphaF(B.alphaF() + (1 - B.alphaF()) * A.alphaF());
  return C;
}

QColor blend(const QColor& B, const QColor& A) {
  QColor C;
  C.setRedF(B.redF() + (1 - B.alphaF()) * A.redF());
  C.setGreenF(B.greenF() + (1 - B.alphaF()) * A.greenF());
  C.setBlueF(B.blueF() + (1 - B.alphaF()) * A.blueF());
  C.setAlphaF(B.alphaF() + (1 - B.alphaF()) * A.alphaF());
  return C;
}

static std::map<int, QColor> colors = boost::assign::map_list_of
  (0, QColor(0, 0, 0, 0))
  (1, QColor(120, 120, 120, 255))
  (2, QColor(117, 176, 73, 255))
  (3, QColor(134, 96, 67, 255))
  (4, QColor(115, 115, 115, 255))
  (48, QColor(115, 115, 115, 255))
  (5, QColor(157, 128, 79, 255))
  (6, QColor(0, 0, 0, 0))
  (7, QColor(84, 84, 84, 255))
  (8, QColor(8, 18, 51, 128))
  (9, QColor(8, 18, 51, 128))
  (10, QColor(255, 90, 0, 255))
  (11, QColor(255, 90, 0, 255))
  (12, QColor(218, 210, 158, 255))
  (13, QColor(136, 126, 126, 255))
  (14, QColor(143, 140, 125, 255))
  (15, QColor(136, 130, 127, 255))
  (16, QColor(115, 115, 115, 255))
  (17, QColor(102, 81, 51, 255))
  (18, QColor(24, 75, 16, 100))
  (20, QColor(64, 64, 64, 64)) //glass
  //(21, QColor(222,50,50,255))
  //(22, QColor(222,136,50,255))
  //(23, QColor(222,222,50,255))
  //(24, QColor(136,222,50,255))
  //(25, QColor(50,222,50,255))
  //(26, QColor(50,222,136,255))
  //(27, QColor(50,222,222,255))
  //(28, QColor(104,163,222,255))
  //(29, QColor(120,120,222,255))
  //(30, QColor(136,50,222,255))
  //(31, QColor(174,74,222,255))
  //(32, QColor(222,50,222,255))
  //(33, QColor(222,50,136,255))
  //(34, QColor(77,77,77,255))
  (35, QColor(222, 222, 222, 255)) //QColor(143,143,143,255);
  //(36, QColor(222,222,222,255))
  (38, QColor(255, 0, 0, 255))
  (37, QColor(255, 255, 0, 255))
  (41, QColor(231, 165, 45, 255))
  (42, QColor(191, 191, 191, 255))
  (43, QColor(200, 200, 200, 255))
  (44, QColor(200, 200, 200, 255))
  (45, QColor(170, 86, 62, 255))
  (46, QColor(160, 83, 65, 255))
  (49, QColor(26, 11, 43, 255))
  (50, QColor(192, 173, 39, 200))
  (51, QColor(200, 133, 24, 200))
  //(52, QColor(245,220,50,255)) unnecessary afaik
  (53, QColor(157, 128, 79, 255))
  (54, QColor(125, 91, 38, 255))
  //(55, QColor(245,220,50,255)) unnecessary afaik
  (56, QColor(129, 140, 143, 255))
  (57, QColor(45, 166, 152, 255))
  (58, QColor(114, 88, 56, 255))
  (59, QColor(146, 192, 0, 255))
  (60, QColor(95, 58, 30, 255))
  (61, QColor(96, 96, 96, 255))
  (62, QColor(96, 96, 96, 255))
  (63, QColor(111, 91, 54, 255))
  (64, QColor(136, 109, 67, 255))
  (65, QColor(23, 18, 8, 32))
  (66, QColor(106, 95, 72, 180))
  (67, QColor(115, 115, 115, 255))
  (71, QColor(191, 191, 191, 255))
  (73, QColor(131, 107, 107, 255))
  (74, QColor(131, 107, 107, 255))
  (75, QColor(23, 18, 8, 32))
  (76, QColor(200, 0, 0, 200))
  (78, QColor(255, 255, 255, 255))
  (79, QColor(17, 23, 33, 51))
  (80, QColor(250, 250, 250, 255))
  (81, QColor(25, 120, 25, 255))
  (82, QColor(151, 157, 169, 255))
  (83, QColor(193, 234, 150, 255))
  (83, QColor(100, 67, 50, 255))
;
