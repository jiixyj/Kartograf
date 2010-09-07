#ifndef _IMG_H
#define _IMG_H

#include "./Color.h"

class IMG{
public:
Color * d;
IMG();
IMG(int width,int height);

void SetPixel(int x,int y,Color q);

Color GetPixel(int x,int y);

Color * GetPPointer(int x,int y);
~IMG();
private:
int w;
int h;
};

#endif  // _IMG_H
