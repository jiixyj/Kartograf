#ifndef _LEVEL_H
#define _LEVEL_H

#include <QtGui>

#include "./render.h"

extern int flip;
extern int Rotate;
extern int cave;
extern int slide;

QColor Blend(QColor A, QColor B, int h);


unsigned long file_size(char *filename);



class Level {
public:
  Level();
  ~Level();

  void Save(char* name);

//read
  int Read(int x, int y, int z, unsigned char * &d, int ret);
  int GetHeight(int x, int y);

//edit
  void Edit(int x, int y, int z, int block, unsigned char * &d); //edit one block

//Color Blend(Color A,Color B,int h);

//load data from chunk file
  const render LoadLevelFromFile(const char * name, const int slice, const bool CWATER, const int cut);

  void MakeNewWorld(const wchar_t  * folder);
  void WriteBuffer(const char * file);

  bool IsBlock(int x, int y, int z);

  const double getlight(int x, int y, int z, double sky, double block, bool CWATER, int slice);

  QColor GetColor(int blockid);


  void AddName(const char * name);

  void End();

  std::string buffer;

  int mapx;
  int mapy;
  int mapz;

  int posx;
  int posy;

  unsigned int count[128];

  int Sheep;
  int Skeleton;
  int Creeper;
  int Zombie;
  int Pig;
  int Cow;
  int Slime;


private:

//char arrays for level data
  unsigned char * blocklight;
  unsigned char * skylight;
  unsigned char * blocks;

  QColor BlockC[256];


  unsigned char * databuffer;





};

#endif  // LEVEL_H
