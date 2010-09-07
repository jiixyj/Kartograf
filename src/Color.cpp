#include "./Color.h"

Color::Color(){
a=0;
r = 255;
g=255;
b=255;
}
Color::Color(unsigned char rr ,unsigned char gg,unsigned char bb,unsigned char aa){
r=rr;g=gg;b=bb;a=aa;

}
Color::~Color() {}

