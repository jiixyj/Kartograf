#ifndef _COLOR_H
#define _COLOR_H

struct Color{
unsigned char r;
unsigned char g;
unsigned char b;
unsigned char a;

Color();
Color(unsigned char rr ,unsigned char gg,unsigned char bb,unsigned char aa);
~Color();
};

#endif  // _COLOR_H
