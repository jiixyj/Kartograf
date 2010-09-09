#ifndef _COLOR_H
#define _COLOR_H

#include <QColor>

class Color : public QColor {
 public:
  Color();
  Color(int r, int g, int b, int a = 255);
  ~Color();
};

#endif  // _COLOR_H
