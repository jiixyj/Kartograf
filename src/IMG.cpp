#include "./IMG.h"

IMG::IMG(){
w = 16;h = 16;
d = new Color[w*h];
}

IMG::IMG(int width,int height){
w = width; h = height;
d = new Color[w*h];
}

void IMG::SetPixel(int x,int y,Color q){
if(x >= 0 && x < w && y >= 0 && y < h)
d[x+y*w] = q;
}

Color IMG::GetPixel(int x,int y){
if(x >= 0 && x < w && y >= 0 && y < h)
return d[x+y*w];
else
return Color(0,0,0,0);
}

Color * IMG::GetPPointer(int x,int y){
if(x >= 0 && x < w && y >= 0 && y < h)
return &d[x+y*w];
else
return 0;
}
IMG::~IMG(){
delete [] d;
}
