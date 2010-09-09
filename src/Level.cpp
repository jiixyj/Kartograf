#include "Level.h"

#include <iostream>
#include <fstream>

#include "zlib.h"
#include "./Color.h"

extern int flip;
extern int Rotate;
extern int daynight;
extern int cave;
extern int exclude;
extern int slide;


Level::~Level() {

}

QColor Blend(QColor B, QColor A, int h) {
  QColor C;
  C.setRedF(B.redF() + (1 - B.alphaF()) * A.redF()*(h / 128.0));
  C.setGreenF(B.greenF() + (1 - B.alphaF()) * A.greenF()*(h / 128.0));
  C.setBlueF(B.blueF() + (1 - B.alphaF()) * A.blueF()*(h / 128.0));
  C.setAlphaF(B.alphaF() + (1 - B.alphaF()) * A.alphaF());
  return C;
}



Level::Level() {

  mapx = 16;
  mapy = 16;
  mapz = 128;

  Sheep = 0;
  Skeleton = 0;
  Creeper = 0;
  Zombie = 0;
  Pig = 0;
  Cow = 0;
  Slime = 0;




  for (int m = 0; m < 128; m++) {
    count[m] = 0;
  }



  for (int b = 0; b < 255; b++) {
    BlockC[b] = Color(0, 0, 0, 0);
  }
  BlockC[0] = Color(255, 255, 255, 0);
  BlockC[1] = Color(120, 120, 120, 255);
  BlockC[2] = Color(117, 176, 73, 255);
  BlockC[3] = Color(134, 96, 67, 255);
  BlockC[4] = Color(115, 115, 115, 255);
  BlockC[48] = Color(115, 115, 115, 255);
  BlockC[5] = Color(157, 128, 79, 255);
  BlockC[6] = Color(120, 120, 120, 0);
  BlockC[7] = Color(84, 84, 84, 255);
  BlockC[8] = Color(38, 92, 255, 51);
  BlockC[9] = Color(38, 92, 255, 51);
  BlockC[10] = Color(255, 90, 0, 255);
  BlockC[11] = Color(255, 90, 0, 255);
  BlockC[12] = Color(218, 210, 158, 255);
  BlockC[13] = Color(136, 126, 126, 255);
  BlockC[14] = Color(143, 140, 125, 255);
  BlockC[15] = Color(136, 130, 127, 255);
  BlockC[16] = Color(115, 115, 115, 255);
  BlockC[17] = Color(102, 81, 51, 255);
  BlockC[18] = Color(60, 192, 41, 100);
  BlockC[20] = Color(255, 255, 255, 64); //glass
  //BlockC[21] = Color(222,50,50,255);
  //BlockC[22] = Color(222,136,50,255);
  //BlockC[23] = Color(222,222,50,255);
  //BlockC[24] = Color(136,222,50,255);
  //BlockC[25] = Color(50,222,50,255);
  //BlockC[26] = Color(50,222,136,255);
  //BlockC[27] = Color(50,222,222,255);
  //BlockC[28] = Color(104,163,222,255);
  //BlockC[29] = Color(120,120,222,255);
  //BlockC[30] = Color(136,50,222,255);
  //BlockC[31] = Color(174,74,222,255);
  //BlockC[32] = Color(222,50,222,255);
  //BlockC[33] = Color(222,50,136,255);
  //BlockC[34] = Color(77,77,77,255);
  BlockC[35] = Color(222, 222, 222, 255); //Color(143,143,143,255);
  //BlockC[36] = Color(222,222,222,255);
  BlockC[38] = Color(255, 0, 0, 255);
  BlockC[37] = Color(255, 255, 0, 255);
  BlockC[41] = Color(231, 165, 45, 255);
  BlockC[42] = Color(191, 191, 191, 255);
  BlockC[43] = Color(200, 200, 200, 255);
  BlockC[44] = Color(200, 200, 200, 255);
  BlockC[45] = Color(170, 86, 62, 255);
  BlockC[46] = Color(160, 83, 65, 255);
  BlockC[49] = Color(26, 11, 43, 255);
  BlockC[50] = Color(245, 220, 50, 200);
  BlockC[51] = Color(255, 170, 30, 200);
  //BlockC[52] = Color(245,220,50,255); unnecessary afaik
  BlockC[53] = Color(157, 128, 79, 255);
  BlockC[54] = Color(125, 91, 38, 255);
  //BlockC[55] = Color(245,220,50,255); unnecessary afaik
  BlockC[56] = Color(129, 140, 143, 255);
  BlockC[57] = Color(45, 166, 152, 255);
  BlockC[58] = Color(114, 88, 56, 255);
  BlockC[59] = Color(146, 192, 0, 255);
  BlockC[60] = Color(95, 58, 30, 255);
  BlockC[61] = Color(96, 96, 96, 255);
  BlockC[62] = Color(96, 96, 96, 255);
  BlockC[63] = Color(111, 91, 54, 255);
  BlockC[64] = Color(136, 109, 67, 255);
  BlockC[65] = Color(181, 140, 64, 32);
  BlockC[66] = Color(150, 134, 102, 180);
  BlockC[67] = Color(115, 115, 115, 255);
  BlockC[71] = Color(191, 191, 191, 255);
  BlockC[73] = Color(131, 107, 107, 255);
  BlockC[74] = Color(131, 107, 107, 255);
  BlockC[75] = Color(181, 140, 64, 32);
  BlockC[76] = Color(255, 0, 0, 200);
  BlockC[78] = Color(255, 255, 255, 255);
  BlockC[79] = Color(83, 113, 163, 51);
  BlockC[80] = Color(250, 250, 250, 255);
  BlockC[81] = Color(25, 120, 25, 255);
  BlockC[82] = Color(151, 157, 169, 255);
  BlockC[83] = Color(193, 234, 150, 255);
  BlockC[83] = Color(100, 67, 50, 255);



}

QColor Level::GetColor(int blockid) {
  return BlockC[blockid];

}

void Level::Edit(int x, int y, int z, int block, unsigned char * &d) {
  if ((x >= mapx) || (x < 0)) return;
  if ((y >= mapy) || (y < 0)) return;
  if ((z >= mapz) || (z < 0)) return;
  d[x*mapz + y*mapz*mapx + z] = block;

}

int Level::Read(int x, int y, int z, unsigned char * &d, int ret) {
  if ((x >= mapx) || (x < 0)) return ret;
  if ((y >= mapy) || (y < 0)) return ret;
  if ((z >= mapz) || (z < 0)) return ret;
  return d[x*mapz + y*mapz*mapx + z];

}

int Level::GetHeight(int x, int y) {
  if (x >= mapx) return 0;
  if (y >= mapy) return 0;
  if (x < 0) return 0;
  if (y < 0) return 0;
  for (int z = mapz; z > 0; z--) {
    if ((Read(x, y, z, blocks, 0) != 0)) {
      return z;
    }

  }
  return 0;

}



const render Level::LoadLevelFromFile(const char * c, const int slide, const bool CWATER, const int cut) {

  render R(slide);
  R.isgood = false;
  long length;

  int p = 0;

  int half = 16 * 16 * 64;
  int full = 16 * 16 * 128;

  blocklight = new unsigned char[mapx*mapy*mapz];
  skylight = new unsigned char[mapx*mapy*mapz];
  blocks = new unsigned char[mapx*mapy*mapz];



  gzFile filein = gzopen(c, "rb");
  while (!filein) {
    filein = gzopen(c, "rb");
  }
//  filein.seekg (0,std::ios::end);
  length = 131072;
//  filein.seekg (0,std::ios::beg);

  databuffer = new unsigned char[length];
  //filein.seekg (0, std::ios::beg);
  int q = gzread(filein, databuffer, length);
  while (q == -1) {
    q = gzread(filein, databuffer, length);
  };
  gzclose(filein);

  if (databuffer[0] == 10 &&
      databuffer[1] == 0 &&
      databuffer[2] == 0 &&
      databuffer[3] == 10 &&
      databuffer[4] == 0 &&
      databuffer[5] == 5 &&
      databuffer[6] == 76 &&
      databuffer[7] == 101 &&
      databuffer[8] == 118 &&
      databuffer[9] == 101 &&
      databuffer[10] == 108) {
    R.isgood = true;
  } else {
    return R;
  }


  int startlight = 0;
  int bstartlight = 0;
  int startterrain = 0;
  //std::cout << "\nprocessing " << length << " bytes\n";

  bool read = false;

  bool debugent = false;

  for (int z = 11; z < length; z++) {
    if (databuffer[z] == 112 && databuffer[z-1] == 101 && databuffer[z-2] == 101 && databuffer[z-3] == 104) {
      Sheep++;
    }
    if (databuffer[z] == 101 && databuffer[z-1] == 105 && databuffer[z-2] == 98 && databuffer[z-3] == 109) {
      Zombie++;
    }
    if (databuffer[z] == 103 && databuffer[z-1] == 105 && databuffer[z-2] == 80) {
      Pig++;
    }
    if (databuffer[z] == 110 && databuffer[z-1] == 111 && databuffer[z-2] == 116 && databuffer[z-3] == 101 && databuffer[z-4] == 108) {
      Skeleton++;
    }
    if (databuffer[z] == 114 && databuffer[z-1] == 101 && databuffer[z-2] == 112 && databuffer[z-3] == 114) {
      Creeper++;
    }
    if (databuffer[z] == 119 && databuffer[z-1] == 111 && databuffer[z-2] == 67) {
      Cow++;
    }
    if (databuffer[z] == 101 && databuffer[z-1] == 109 && databuffer[z-2] == 105 && databuffer[z-3] == 108 && databuffer[z-4] == 83) {
      Slime++;
    }
    if (databuffer[z] == 115 && databuffer[z-1] == 111 && databuffer[z-2] == 80 && databuffer[z-3] == 120) {
      long long Int32 = 0;
      bool neg = false;
      if (databuffer[z+1] == 255)neg = true;
      //Int32 += (int)databuffer[z+3]*256;
      if (neg) {
        Int32 += -(255 - (int)databuffer[z+2]) * 256 * 256;
        Int32 += -(255 - (int)databuffer[z+3]) * 256;
        Int32 += -(255 - (int)databuffer[z+4]);
      } else {
        Int32 += (int)databuffer[z+2] * 256 * 256;
        Int32 += (int)databuffer[z+3] * 256;
        Int32 += (int)databuffer[z+4] + 1;
      }
      R.x = Int32;
    }
    if (databuffer[z] == 115 && databuffer[z-1] == 111 && databuffer[z-2] == 80 && databuffer[z-3] == 122) {
      long long Int32 = 0;
      bool neg = false;
      if (databuffer[z+1] == 255)neg = true;
      if (neg) {
        Int32 += -(255 - (int)databuffer[z+2]) * 256 * 256;
        Int32 += -(255 - (int)databuffer[z+3]) * 256;
        Int32 += -(255 - (int)databuffer[z+4]);
      } else {
        Int32 += (int)databuffer[z+2] * 256 * 256;
        Int32 += (int)databuffer[z+3] * 256;
        Int32 += (int)databuffer[z+4] + 1;
      }
      R.y = Int32;
    }
    if (databuffer[z] == 115 && databuffer[z-1] == 107 && databuffer[z-2] == 99 && databuffer[z-3] == 111 && databuffer[z-4] == 108 && databuffer[z-5] == 66 && databuffer[z-6] == 6 && databuffer[z-7] == 0) {
      startterrain = z + 5;
    }
    if (z > startterrain + 10 && startterrain != 0) break;

    if (databuffer[z] == 116 && databuffer[z-1] == 104 && databuffer[z-2] == 103 && databuffer[z-3] == 105 && databuffer[z-4] == 76 && databuffer[z-5] == 121) {
      startlight = z + 5;
    }
    if (databuffer[z] == 116 && databuffer[z-1] == 104 && databuffer[z-2] == 103 && databuffer[z-3] == 105 && databuffer[z-4] == 76 && databuffer[z-5] == 107) {
      bstartlight = z + 5;
    }
  }


  if (R.x > 10000000 || R.y > 10000000) {
    R.isgood = false;
    return R;
  }


  if (debugent) {
    std::string poo = "ENTS";
    std::ofstream outfile(poo.c_str(), std::ofstream::binary);
    outfile.write((const char*)databuffer, length);
    outfile.close();
  }


  /*for(int x = 0;x< 16;x++){
  for(int y = 0;y< 16;y++){
  for(int z = 0;z< 128;z++){
  Edit(x,y,z,databuffer[startterrain + x*128 + y*128*16 + z],blocks);
  //count[databuffer[startterrain + x*128 + y*128*16 + z]]++;
  }}}*/

  for (int pss = 0; pss < 32768; pss++) {

    blocks[pss] = databuffer[startterrain + pss];
    count[databuffer[startterrain + pss]]++;
  }


  for (int x = 0; x < 16; x++) {
    for (int y = 0; y < 16; y++) {
      for (int z = 0; z < 128; z++) {

        int half = x * 64 + y * 64 * 16 + z * 0.5;
        int l;
        if (z % 2 == 0)
          l = (int)databuffer[startlight + half] % 16;
        else
          l = (int)databuffer[startlight + half] / 16;
        Edit(x, y, z, l, skylight);

      }
    }
  }

  QColor waste;

  for (int x = 0; x < 16; x++) {
    for (int y = 0; y < 16; y++) {
      for (int z = 0; z < 128; z++) {

        int half = x * 64 + y * 64 * 16 + z * 0.5;
        int l;
        if (z % 2 == 0)
          l = (int)databuffer[bstartlight + half] % 16;
        else
          l = (int)databuffer[bstartlight + half] / 16;
        Edit(x, y, z, l, blocklight);

      }
    }
  }



  if (slide < 69) {
    for (int x = 0; x < 16; x++) {
      for (int y = 0; y < 16; y++) {

        int l;

        QColor t = QColor::fromRgba(R.Q.pixel(y, x));
        if (cave) {
          for (int z = 127; z > 0; z--) {
            int rr = Read(x, y, z, blocks, 0);
            int blight = Read(x, y, z, blocklight, 0);
            if (Read(x, y, z, skylight, 15) == 0 && IsBlock(x, y, z)) {
              QColor B;



              if (blight > 0) blight++;

              if (Read(x, y, z, skylight, 15) == 0 && IsBlock(x, y, z) && blight == 0)
                blight = 1;

              if (Read(x, y, z, skylight, 15) != 0 || rr == 8 || rr == 9 || rr == 79)
                blight = 0;



              int bbb = 128 - (blight * 8);

              int rt = (bbb > 0) * (bbb <= 32) * 255 + (bbb > 32) * (bbb < 64) * (32 - (bbb - 32)) * 8;
              int gt = (bbb > 96) * (bbb < 128) * (32 - (bbb - 96)) * 8 + (bbb > 0) * (bbb < 32) * (bbb) * 8 + (bbb >= 32) * (bbb <= 96) * 255;
              int bt = (bbb >= 96) * (bbb < 128) * 255 + (bbb > 64) * (bbb < 96) * (bbb - 64) * 8;

              if (rr == 10 || rr == 11) {
                rt = 255;
                gt = 255;
                bt = 0;
              }


              int zzz = 128 - z;

              B.setRed(rt * 2);
              B.setGreen(gt * 2);
              B.setBlue(bt * 2);
              B.setAlpha((blight > 0) * 255);


              int zop = z * 0.5 + 64;

              t = Blend(t, B, (zop));
              if (t.alpha() == 255) break;
            }
          }
        } else if (slide == -1) {
          for (int z = 127; z >= 0; z--) {
            int rr = Read(x, y, z, blocks, 0);
            if ((rr != 0 && (exclude == 0 || rr == exclude)) || (!CWATER && (rr != 8 && rr != 9))) {
              switch (daynight) {
              case(0):
                l = getlight(x, y, z + 1, 1, 1, CWATER, slide) * 4;
                break;
              case(1):
                l = getlight(x, y, z + 1, 0.5, 1, CWATER, slide) * 4;
                break;
              case(2):
                l = getlight(x, y, z + 1, 0, 1, CWATER, slide) * 4;
                break;
              }
              if (exclude != 0) l = 128;
              int br = 0.8 * l + 0.5 * z;
              if (br > 128) br = 128;

              if (rr == 10 || rr == 11 || rr == 50 || rr == 51 || rr == 76) {
                t = Blend(t, GetColor(rr), 128);
              } else if ((rr == 8 || rr == 9 || rr == 79) && CWATER) {
                QColor P = GetColor(rr);
                if (Read(x, y, z + 1, blocks, 0) == 0) {
                  P.setAlpha(128);
                  t = Blend(t, P, br);
                }
              } else {
                t = Blend(t, GetColor(rr), br);
              }
            }
            //if(z == 0)
            //t = BlockC[10];
            if (t.alpha() >= 255) {
              z = -10;
              t.setAlpha(255);
            }
          }
        } else if (slide == -3) {
          for (int z = 127; z >= 0; z--) {
            int rr = Read(x, y, z, blocks, 0);
            if ((rr != 0 && ((rr != 8 && rr != 9) || !CWATER) && (exclude == 0 || rr == exclude))) {

              int zzz = 128 - z;
              t.setRed((zzz > 0)*(zzz <= 32) * 255 + (zzz > 32)*(zzz < 64)*(32 - (zzz - 32)) * 8);
              t.setGreen((zzz > 96)*(zzz < 128)*(32 - (zzz - 96)) * 8 + (zzz > 0)*(zzz < 32)*(zzz) * 8 + (zzz >= 32)*(zzz <= 96) * 255);
              t.setBlue((zzz >= 96)*(zzz < 128) * 255 + (zzz > 64)*(zzz < 96)*(zzz - 64) * 8);
              t.setAlpha(255);
              z = -10;
            }
          }
        } else if (slide == -4) {
          for (int z = 127; z >= 0; z--) {
            int rr = Read(x, y, z, blocks, 0);
            if ((rr != 0 && ((rr != 8 && rr != 9) || !CWATER) && (exclude == 0 || rr == exclude))) {

              t.setRed(z * 2);
              t.setGreen(z * 2);
              t.setBlue(z * 2);
              t.setAlpha(255);
              z = -10;
            }
          }
        } else if (slide == -5) {
          for (int z = 127; z >= 0; z--) {
            int rr = Read(x, y, z, blocks, 0);

            if (rr == 14)t = Blend(t, Color(0, 255, 0, 32), 128);
            if (rr == 15)t = Blend(t, Color(255, 0, 0, 32), 128);
            if (rr == 16)t = Blend(t, Color(0, 0, 0, 32), 128);
            if (rr == 56)t = Blend(t, Color(0, 0, 255, 32), 128);

            if (t.alpha() > 255) {
              z = -10;
              t.setAlpha(255);
            }
          }
        } else {
          int rr = Read(x, y, cut, blocks, 0);
          t = GetColor(rr);

        }
// std::cout << "t: " << (t.red()) << " " <<  (t.green()) << " "
//                    << (t.blue()) << " " << (t.alpha()) << std::endl;
//                   std::cout << std::endl;
        R.Q.setPixel(y, x, t.rgba());
      }
    }
  } else {
    if (slide == 70) {
      for (int z = 128; z > -1; z--) {
        for (int xr = 0; xr < 16; xr++) {
          for (int yr = !(Rotate && !flip) * 15; yr != -1 + (Rotate && !flip) * 17; yr += 1 - !(Rotate && !flip) * 2) {



            int xo, yo;

            int ff = -1;
            int  ff2 = 1;

            if (Rotate) {

              if (!flip) {
                yo =  yr;
                xo =  15 - xr;
                ff = -1;
                ff2 = -1;
              } else {
                ff = 1;
                xo = xr;
              }

            } else {
              xo = 15 - xr;

            }



            if (flip) {

              if (!Rotate) {
                xo = xr;
                yo = 15 - yr;
                ff = 1;
                ff2 = 1;
              } else {
                yo =  yr;
                ff2 = -1;
              }

            } else {
              yo = 15 - yr;

            }

            int x = xr;
            int y = yr;
            if (Rotate && !flip)
              y =  15 - yr;


            int rr = Read(xo, yo, z, blocks, 0);
            bool f, t, r;
            f = IsBlock(xo - ff, yo, z);
            t = IsBlock(xo, yo, z + 1);
            r = IsBlock(xo, yo - ff2, z);




            int rrx = 16 + (y) - (x);
            int rry = 0 + (y) + (x);
            QColor t2;
            QColor t3;
            QColor t4;
            QColor t1 = QColor::fromRgba(R.Q.pixel(rry, (128 - z) + rrx));

            if (rrx + 1 + (128 - z) < 160)
              t2 = QColor::fromRgba(R.Q.pixel(rry, ((128 - z) + rrx + 1)));
            else
              t2 = waste;



            if (rry + 1 < 34 && rrx + 1 + (128 - z) < 160)
              t3 = QColor::fromRgba(R.Q.pixel(rry + 1, ((128 - z) + rrx + 1)));
            else
              t2 = waste;

            t4 = QColor::fromRgba(R.Q.pixel(rry + 1, ((128 - z) + rrx)));






            int here, top, left, right;
            if (exclude == 0 && !cave) {
              switch (daynight) {
              case(0):
                here = getlight(xo, yo, z, 1, 1, CWATER, slide) * 4;
                top = getlight(xo, yo, z + 1, 1, 1, CWATER, slide) * 4;
                left = getlight(xo - ff, yo, z, 1, 1, CWATER, slide) * 4;
                right = getlight(xo, yo - ff2, z, 1, 1, CWATER, slide) * 4;
                break;
              case(1):
                here = getlight(xo, yo, z, 0.5, 1, CWATER, slide) * 4;
                top = getlight(xo, yo, z + 1, 0.5, 1, CWATER, slide) * 4;
                left = getlight(xo - ff, yo, z, 0.5, 1, CWATER, slide) * 4;
                right = getlight(xo, yo - ff2, z, 0.5, 1, CWATER, slide) * 4;
                break;
              case(2):
                here = getlight(xo, yo, z, 0, 1, CWATER, slide) * 4;
                top = getlight(xo, yo, z + 1, 0, 1, CWATER, slide) * 4;
                left = getlight(xo - ff, yo, z, 0, 1, CWATER, slide) * 4;
                right = getlight(xo, yo - ff2, z, 0, 1, CWATER, slide) * 4;
                break;
              }
            }

            if (exclude != 0) {
              top = 128;
              here = 128;
              right = 128;
              left = 128;
            }
            if (cave) {
              QColor B;

              int blight = Read(xo, yo, z, blocklight, 0);
              if (blight > 0) blight++;

              if (Read(xo, yo, z, skylight, 15) == 0 && IsBlock(xo, yo, z) && blight == 0)
                blight = 1;

              if (Read(xo, yo, z, skylight, 15) != 0 || rr == 8 || rr == 9 || rr == 79)
                blight = 0;



              int bbb = 128 - (blight * 8);

              int rt = (bbb > 0) * (bbb <= 32) * 255 + (bbb > 32) * (bbb < 64) * (32 - (bbb - 32)) * 8;
              int gt = (bbb > 96) * (bbb < 128) * (32 - (bbb - 96)) * 8 + (bbb > 0) * (bbb < 32) * (bbb) * 8 + (bbb >= 32) * (bbb <= 96) * 255;
              int bt = (bbb >= 96) * (bbb < 128) * 255 + (bbb > 64) * (bbb < 96) * (bbb - 64) * 8;

              if (rr == 10 || rr == 11) {
                rt = 255;
                gt = 255;
                bt = 0;
              }


              int zzz = 128 - z;

              B.setRed(rt * 2);
              B.setGreen(gt * 2);
              B.setBlue(bt * 2);
              B.setAlpha((blight > 0) * 255);


              int zop = z * 0.5 + 64;

              t1 = Blend(t1, B, (zop));
              t2 = Blend(t2, B, (zop) * 0.5);
              t3 = Blend(t3, B, (zop) * 0.65);
              t4 = Blend(t4, B, (zop) * 0.8);

            }

            if (!cave) {
              if (rr != 0 && rr != 78 && (f || t ||  r || exclude != 0) && (exclude == 0 || rr == exclude)) {
                if (rr == 76 || rr == 75 || rr == 6 || rr == 37 || rr == 38 || rr == 39 || rr == 40 || rr == 51 || rr == 50 || rr == 59 || rr == 63 || rr == 65 || rr == 66) {
                  if (rr == 50 || rr == 51 || rr == 76) {
                    t2 = Blend(t2, GetColor(rr), 128);
                  } else {
                    t2 = Blend(t2, GetColor(rr), here * 0.8 + z * 0.4);
                  }
                } else {
                  if (rr == 10 || rr == 11) {
                    t1 = Blend(t1, GetColor(rr), (128));
                    t2 = Blend(t2, GetColor(rr), (128) * 0.5);
                    t3 = Blend(t3, GetColor(rr), (128) * 0.65);
                    t4 = Blend(t4, GetColor(rr), (128) * 0.8);

                  } else {
                    if (rr == 8 || rr == 9 || rr == 79) {
                      QColor P = GetColor(rr);
                      if (Read(xo, yo, z + 1, blocks, 0) == 0 || Read(xo + 1, yo, z, blocks, -1) == 0 || Read(xo, yo + 1, z, blocks, -1) == 0) {
                        P.setAlpha(128);
                        t1 = Blend(t1, P, (here * 0.8 + z * 0.4));
                        t2 = Blend(t2, P, (here * 0.8 + z * 0.4) * 0.65);
                        t3 = Blend(t3, P, (here * 0.8 + z * 0.4) * 0.5);
                        t4 = Blend(t4, P, (here * 0.8 + z * 0.4) * 0.8);
                      }


                    } else if (rr == 2) {
                      if (Read(xo, yo, z + 1, blocks, 0) == 78)
                        t1 = Blend(t1, Color(255, 255, 255, 255), (top * 0.8 + z * 0.4));
                      else
                        t1 = Blend(t1, GetColor(2), (top * 0.8 + z * 0.4));
                      t2 = Blend(t2, GetColor(3), (left * 0.8 + z * 0.4) * 0.65);
                      t3 = Blend(t3, GetColor(3), (right * 0.8 + z * 0.4) * 0.5);
                      if (Read(xo, yo, z + 1, blocks, 0) == 78)
                        t4 = Blend(t4, Color(255, 255, 255, 255), (top * 0.8 + z * 0.4) * 0.8);
                      else
                        t4 = Blend(t4, GetColor(2), (top * 0.8 + z * 0.4) * 0.8);
                    } else {
                      if (Read(xo, yo, z + 1, blocks, 0) == 78)
                        t1 = Blend(t1, Color(255, 255, 255, 255), (top * 0.8 + z * 0.4));
                      else
                        t1 = Blend(t1, GetColor(rr), (top * 0.8 + z * 0.4));
                      t2 = Blend(t2, GetColor(rr), (left * 0.8 + z * 0.4) * 0.75);
                      t3 = Blend(t3, GetColor(rr), (right * 0.8 + z * 0.4) * 0.5);
                      if (Read(xo, yo, z + 1, blocks, 0) == 78)
                        t4 = Blend(t4, Color(255, 255, 255, 255), (top * 0.8 + z * 0.4) * 0.8);
                      else
                        t4 = Blend(t4, GetColor(rr), (top * 0.8 + z * 0.4) * 0.8);

                    }


                  }
                }
              }
            }

            R.Q.setPixel(rry, (128 - z) + rrx, t1.rgba());
            R.Q.setPixel(rry, (128 - z) + rrx + 1, t2.rgba());
            R.Q.setPixel(rry + 1, (128 - z) + rrx + 1, t3.rgba());
            R.Q.setPixel(rry + 1, (128 - z) + rrx, t4.rgba());

          }
        }
      }

    } else if (slide == 69) {
      for (int z = 128; z > -1; z--) {
        for (int xr = 15 - (flip == 1) * 15; xr != -1 + (flip == 1) * 17; xr += -1 + (flip == 1) * 2) {
          for (int yr = 0; yr < 16; yr++) {

            int xo, yo;

            if (Rotate == 1) {
              xo = yr;
              yo = xr;
            } else {
              xo = xr;
              yo = yr;
            }

            int x = xr;
            int y = yr;
            int ff = 1;
            if (flip == 1) {
              x = 15 - xr;
              //y = 15-yo;
              ff = -1;
            }



            int rr = Read(xo, yo, z, blocks, 0);
            bool f, t;
            t = IsBlock(xo, yo, z + 1);
            if (Rotate == 1) {
              if (flip == 1) {
                f = IsBlock(xo, yo - 1, z);
              } else {
                f = IsBlock(xo, yo + 1, z);
              }
            } else {
              if (flip == 1) {
                f = IsBlock(xo - 1, yo, z);

              } else {
                f = IsBlock(xo + 1, yo, z);
              }
            }

            QColor t1 = QColor::fromRgba(R.Q.pixel(y, ((128 - z) + x)));
            QColor t2;
            if (z > 1)
              t2 = QColor::fromRgba(R.Q.pixel(y, (128 - z) + x + 1));
            else
              t2 = QColor();

            int here, top, front;
            if (exclude == 0 && !cave) {
              switch (daynight) {
              case(0):
                here = getlight(xo, yo, z, 1, 1, CWATER, slide) * 4;
                top = getlight(xo, yo, z + 1, 1, 1, CWATER, slide) * 4;
                front = getlight(xo + ff * (Rotate != 1), yo + ff * (Rotate == 1), z, 1, 1, CWATER, slide) * 4;
                break;
              case(1):
                here = getlight(xo, yo, z, 0.5, 1, CWATER, slide) * 4;
                top = getlight(xo, yo, z + 1, 0.5, 1, CWATER, slide) * 4;
                front = getlight(xo + ff * (Rotate != 1), yo + ff * (Rotate == 1), z, 0.5, 1, CWATER, slide) * 4;
                break;
              case(2):
                here = getlight(xo, yo, z, 0, 1, CWATER, slide) * 4;
                top = getlight(xo, yo, z + 1, 0, 1, CWATER, slide) * 4;
                front = getlight(xo + ff * (Rotate != 1), yo + ff * (Rotate == 1), z, 0, 1, CWATER, slide) * 4;
                break;
              }
            }

            if (exclude != 0) {
              top = 128;
              here = 128;
              front = 128;
            }
            if (cave) {
              QColor B;

              int blight = Read(xo, yo, z, blocklight, 0);
              if (blight > 0) blight++;

              if (Read(xo, yo, z, skylight, 15) == 0 && IsBlock(xo, yo, z))
                blight = 1;

              if (Read(xo, yo, z, skylight, 15) != 0 || rr == 8 || rr == 9)
                blight = 0;

              int zzz = 128 - z;
              B.setRed((zzz > 0)*(zzz <= 32) * 255 + (zzz > 32)*(zzz < 64)*(32 - (zzz - 32)) * 8);
              B.setGreen((zzz > 96)*(zzz < 128)*(32 - (zzz - 96)) * 8 + (zzz > 0)*(zzz < 32)*(zzz) * 8 + (zzz >= 32)*(zzz <= 96) * 255);
              B.setBlue((zzz >= 96)*(zzz < 128) * 255 + (zzz > 64)*(zzz < 96)*(zzz - 64) * 8);
              B.setAlpha((blight > 0) * 255);




              t1 = Blend(t1, B, (4 * blight + 128));
              t2 = Blend(t2, B, (4 * blight + 128) * 0.5);

            }

            if (!cave) {
              if (rr != 0  && rr != 78 && (t || f || exclude != 0) && (exclude == 0 || rr == exclude)) {
                if (rr == 8 || rr == 9 || rr == 79) {
                  QColor P = GetColor(rr);
                  if (Read(xo, yo, z + 1, blocks, 0) == 0) {
                    P.setAlpha(128);
                    t1 = Blend(t1, P, top);
                  }


                } else if (rr == 6 || rr == 75 || rr == 76 || rr == 37 || rr == 38 || rr == 39 || rr == 40 || rr == 51 || rr == 50 || rr == 59 || rr == 63 || rr == 65 || rr == 66) {
                  if (rr == 50 || rr == 51 || rr == 76) {
                    t2 = Blend(t2, GetColor(rr), 128);
                  } else {
                    t2 = Blend(t2, GetColor(rr), here * 0.8 + z * 0.4);
                  }
                } else {
                  if (rr == 10 || rr == 11) {
                    t1 = Blend(t1, GetColor(rr), 128);
                    t2 = Blend(t2, GetColor(rr), 128 * 0.5);

                  } else {
                    if (rr == 2) {
                      if (Read(xo, yo, z + 1, blocks, 0) == 78)
                        t1 = Blend(t1, Color(255, 255, 255, 255), top * 0.8 + z * 0.4);
                      else
                        t1 = Blend(t1, GetColor(2), (top * 0.8 + z * 0.4));
                      t2 = Blend(t2, GetColor(3), (front * 0.8 + z * 0.4) * 0.75);
                    } else {
                      if (Read(xo, yo, z + 1, blocks, 0) == 78)
                        t1 = Blend(t1, Color(255, 255, 255, 255), (top * 0.8 + z * 0.4));
                      else
                        t1 = Blend(t1, GetColor(rr), (top * 0.8 + z * 0.4));
                      t2 = Blend(t2, GetColor(rr), (front * 0.8 + z * 0.4) * 0.75);

                    }


                  }
                }
              }
            }

          }
        }
      }
    }

  }
  delete[]  skylight;
  delete[]  blocks;
  delete[] blocklight;
  delete[] databuffer;
  return R;
}


bool Level::IsBlock(int x, int y, int z) {
  int qq = Read(x, y, z, blocks, 0);
  return (qq == 83 || (qq > 74 && qq < 80) || (qq > 67 && qq < 73) || (qq > 62 && qq < 67) || qq == 10 || qq == 20 || qq == 0 || qq == 37 || qq == 38 || qq == 39 || qq == 40 || qq == 50 || qq == 18 || qq == 8 || qq == 9);
}


const double Level::getlight(int x, int y, int z, double sky, double block, bool CWATER, int slide) {
  double b = (double)Read(x, y, z, skylight, 15 * sky) * sky;

  double s = (double)Read(x, y, z, blocklight, 0) * block;
  if (!CWATER && slide == -1) s = 15 * sky;
  if (s > b)
    return s + 1;
  else
    return b + 1;
}
