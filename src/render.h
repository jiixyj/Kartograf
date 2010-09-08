#ifndef _RENDER_H
#define _RENDER_H

#include <QtGui>

extern int flip;
extern int Rotate;
extern int cave;
extern int slide;

struct render {
  render();
  render(int bb);
  //IMG  i;
  long long x;
  long long y;
  //char rawx[4];
  QImage Q;
  //char rawy[4];
  ~render();
  bool isgood;
  int GetXc() {
    return y - x;
  }

  int GetYc() {
    return x + y;
  }

  bool operator < (const render& other) {
    if (slide == 70) {
      int xx = (x) - (y);
      int yy = (x) + (y);
      int xx2 = (other.x) - (other.y);
      int yy2 = (other.x) + (other.y);
      if (flip == 1 && Rotate == 1) return xx < xx2;
      if (flip == 1) return yy > yy2;
      if (Rotate == 1)	return yy < yy2;
      return xx > xx2;


    } else {
      if (flip == 1) {
        if (Rotate == 0)
          return y > other.y;
        else
          return x > other.x;
      }

      else {
        if (Rotate == 0)
          return y < other.y;
        else
          return x < other.x;
      }

    }
  }

};

#endif  // _RENDER_H
