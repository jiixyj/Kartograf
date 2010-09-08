#include "Level.h"
#include "resource.h"

#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>

#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include <list>
#include <iostream>
#include <fstream>
#include "global.h"

#include <QtGui>

//My changes:
//ListFiles rewrited to posix mode
//gui is destroy [oh god]
//only console
//what the hell is wchar_t??
//by Firemark :)

int CWorld = 0;

int cut = 0;
bool RENDER = false;
bool water = true;

void Dostuff() {
  #ifdef Q_WS_WIN
    QSettings ini(QSettings::IniFormat,
                  QSettings::UserScope,
                  QCoreApplication::organizationName(),
                  QCoreApplication::applicationName());
    QDir dir = QDir(QFileInfo(ini.fileName()).absolutePath());
  #else
    QDir dir = QDir::home();
  #endif
  if (!dir.cd(QString(".minecraft/saves/World") + QString::number(CWorld))) {
    std::cerr << "Tried dir: "
              << dir.canonicalPath().toStdString() << std::endl;
    qFatal("Minecraft is not installed!");
  }

  std::stringstream ss;
  ss << "map" << CWorld;
  std::string name = ss.str();
  std::string png = ".png";
  std::string A = dir.canonicalPath().toStdString();

  std::cout << A << " " << name << std::endl;
  //return 0;

  // std::string C = L"C:\\Users\\Harm\\Documents\\Visual Studio 2008\\Projects\\WinCartograph\\test\\edit";

  //wcout << A.c_str();
  //return 0;
  std::list<render> renderblocks;

  int cc = 0;
  //int lool = 0;

  Level foo;

  //decompress_one_file("C:\\Users\\Harm\\AppData\\Roaming\\.minecraft\\saves\\World3\\4\\1c\\c.4.-g.dat","level_in");
  //std::cout << "\nRendering chunks..";

  std::cout << "2)Unzip and draw..." << std::endl;
  dir.setFilter(QDir::Files);
  QDirIterator it(dir, QDirIterator::Subdirectories);
  while (it.hasNext()) {
       const render * temp = foo.LoadLevelFromFile(it.next().toAscii().data(),slide,water,cut);
       if(temp->isgood){
        cc++;
        renderblocks.push_back(*temp);
       }
      //}

        }
    std::cout << "Done!" << std::endl;
  //}
    //int lool2 = renderblocks.size();

  int minx = 100000000;
  int miny = 100000000;
  int maxx = -100000000;
  int maxy = -100000000;
  for (std::list<render>::iterator pit=renderblocks.begin();pit!=renderblocks.end();pit++){
    //std::cout << "  " << it->x << " " << it->y;
    if(abs(pit->x) < 65536 && abs(pit->y) < 65536){

      if(pit->x < minx)
      minx = pit->x;
    if(pit->y < miny)
      miny = pit->y;

    if(pit->x > maxx)
      maxx = pit->x;
    if(pit->y > maxy)
      maxy = pit->y;

    }

  }
  //std::cout << "\n\nMAXIMUM X = " << maxx << " MAXIMUM Y = " << maxy;
  //std::cout << "\n\nMINIMUM X = " << minx << " MINIMUM Y = " << miny;
  int imageheight,imagewidth;



  if(slide == 70){
    int cxx = ((((maxx-minx))+(maxy-miny)))*0.5;
    int cyy = (((maxx-minx))+(maxy-miny))*0.5;

    imageheight = (cxx)*32 + 288;
    imagewidth = (cyy)*32 + 288;

    }else{
  imageheight = (maxx-minx)*16 + 144;
  imagewidth = (maxy-miny)*16 + 144;
  }
  IMG * MAP;
  if(slide == 70)
  MAP = new IMG(imageheight,imagewidth+128);
  else if(slide == 69)
  MAP = new IMG(imageheight,imagewidth);
  else
  MAP = new IMG(imageheight,imagewidth);

  //std::cout << "\nAssembling image...";
  //if(slide == 69){
  renderblocks.sort();
  renderblocks.reverse();
  //}

  //int * xytt;

  //xytt = new int[10000];
  //for(int b = 0;b < 10000;b++){
  //xytt[b] = 0;
  //}
  for (std::list<render>::iterator rit=renderblocks.begin();rit!=renderblocks.end();rit++){

    //total++;
    int xb = rit->x;
    int yb = rit->y;

    int xx = (xb-minx)*16;
    int yy = (yb-miny)*16;

    if(slide == 70){
    if(flip == 1)
    yy = abs((yb-miny)-(maxy-miny))*16;
    if(Rotate == 0)
    xx = abs((xb-minx)-(maxx-minx))*16;



    int temx = xx;
    int temy = yy;
    xx = imageheight/2+(temx)-(temy);
    yy = (temx)+(temy)+128;

  }

  if(slide == 69){

    if(flip == 1){
      if(Rotate == 0){
      //xx = (xb-minx)*16;
      yy = abs((yb-miny)-(maxy-miny))*16;
      }else{
      xx = (yb-miny)*16;
      yy = abs((xb-minx)-(maxx-minx))*16;

      }
    }else{
    if(Rotate == 1){
    xx = (yb-miny)*16;
    yy = (xb-minx)*16;
    }
    }
    }
  //xytt[(xx/16)+(yy/16)*100]++;
  //if(xytt[(xx/16)+(yy/16)*100] > 1)
  //myfile << "X:" << xb << " Y:" << yb << " " << "xx:" << xx << " yy:" << yy << "  " << xytt[(xx/16)+(yy/16)*100] << std::endl;
  //std::cout << xx << "_" << yy << " ";
  if(xx >= 0 && yy >= 0 && xx < 40000 && yy < 40000){
    if(slide == 70){


  for(int xo = 0;xo < 33;xo++){
  for(int yo = 0;yo < 160;yo++){
    int x = xo;
    int y = yo;
  MAP->SetPixel(x+xx,y+yy-128,Blend(MAP->GetPixel(x+xx,y+yy-128),rit->Q->GetPixel(x,y),128));
  }}


    }else{
  for(int xo = 0;xo < 16;xo++){
  for(int yo = 0;yo < 16+(slide > 68)*128;yo++){
    int x = xo;
    int y = yo;

    //MAP.SetPixel(x+xx,y+yy,sf::Color(125,255,0,255));

    if(slide == 69)
    MAP->SetPixel(x+xx,y+yy,Blend(MAP->GetPixel(x+xx,y+yy),rit->Q->GetPixel(x,y),128));
    else if(slide == 70)
    MAP->SetPixel(x+xx,y+yy-128,Blend(MAP->GetPixel(x+xx,y+yy-128),rit->Q->GetPixel(x,y),128));
    else
    MAP->SetPixel(x+xx,y+yy,rit->Q->GetPixel(x,y));

  }
  }}
  }}



  std::string txtname = "./"+name+".txt";

  //std::cout << "\n\nERRORS: " << errors << "\nTOTAL: " << total;
  switch(slide){
    case(-1):
      name.append("-normal");
      if(!water)name.append("-nowater");
    break;
    case(-3):
      name.append("-heightcolor");
      if(!water)name.append("-nowater");
    break;
    case(-4):
      name.append("-heightgray");
      if(!water)name.append("-nowater");
    break;
    case(-5):
      name.append("-ore");
    break;
    case(69):
      name.append("-oblique");
      if(!water)name.append("-nowater");
    break;
    case(70):
      name.append("-obliqueangle");
      if(!water)name.append("-nowater");
    break;
    case(64):
    {
    std::string s;
    std::stringstream out;
    out << cut;
    s = out.str();
      name.append("-slice-");
      name += s;
    }
    break;


  }
  //if(flip)
  //name.append("-flipped");
  //if(Rotate)
  //name.append("-rotated");

  if(slide == -1 || slide > 68){
  if(flip)
  name.append("-flipped");
  if(Rotate)
  name.append("-rotated");
  if(cave)
    name.append("-cave");



  switch(daynight){
  case(0):
    name.append("-day");
  break;
  case(1):
    name.append("-day/night");
  break;
  case(2):
    name.append("-night");
  break;
  }
  }

  if(exclude != 0){
    name.append("-only-");
    std::string s;
    std::stringstream out;
    out << exclude;
    s = out.str();
    name += s;

  }




  std::string exit ="./" + name + png;

  std::cout << "3)Save image" << std::endl;
  QImage Output(reinterpret_cast<uchar*>(MAP->d),imageheight,imagewidth,QImage::Format_ARGB32);
        Output.save(QString::fromStdString(exit));

  delete MAP;
  //delete MAP;

  //remove("level_in");
  renderblocks.clear();
  std::cout << "4)Save txt" << std::endl;

  std::ofstream values( txtname.c_str() );
  if (values.is_open())
  {
    values << "Map surface is: " << cc*16*16 << " square meters";
    values << std::endl;
    values << "Block amounts:";
    values << std::endl;
    values << "Air: " << foo.count[0] << std::endl;
    values << "Stone: " << foo.count[1] << std::endl;
    values << "Grass: " << foo.count[2] << std::endl;
    values << "Dirt: " << foo.count[3] << std::endl;
    values << "Snow: " << foo.count[78] << std::endl;
    values << std::endl;
    values << "Water: " << foo.count[8]+foo.count[9] << std::endl;
    values << "Ice: " << foo.count[79] << std::endl;
    values << "Lava: " <<  foo.count[10]+foo.count[11] << std::endl;
    values << std::endl;
    values << "Obisidian: " <<  foo.count[49] << std::endl;
    values << std::endl;
    values << "Trunk: " << foo.count[17] << std::endl;
    values << "Leaves: " << foo.count[18] << std::endl;
    values << "Wood: " << foo.count[5] << std::endl;
    values << "Cactus: " << foo.count[81] << std::endl;
    values << std::endl;
    values << "Sand: " << foo.count[12] << std::endl;
    values << "Gravel: " << foo.count[13] << std::endl;
    values << "Clay: " << foo.count[82] << std::endl;
    values << std::endl;
    values << "Gold Ore: " << foo.count[14] << std::endl;
    values << "Iron Ore: " << foo.count[15] << std::endl;
    values << "Coal Ore: " << foo.count[16] << std::endl;
    values << "Diamond Ore: " << foo.count[56] << std::endl;
    values << "Redstone: " << foo.count[73]+foo.count[74] << std::endl;
    values << std::endl;
    values << "Cobble: " << foo.count[4] << std::endl;
    values << "Glass: " << foo.count[20] << std::endl;
    values << "Cloth: " << foo.count[35] << std::endl;
    values << "Gold: " << foo.count[41] << std::endl;
    values << "Iron: " << foo.count[42] << std::endl;
    values << "Diamond: " << foo.count[57] << std::endl;
    values << std::endl;
    values << "Farmland: " << foo.count[60] << std::endl;
    values << "Crops: " << foo.count[59] << std::endl;
    values << "Reed: " << foo.count[83] << std::endl;
    values << "Torch: " << foo.count[50] << std::endl;
    values << "CraftTable: " << foo.count[58] << std::endl;
    values << "Chest: " << foo.count[54] << std::endl;
    values << "Furnace: " << foo.count[61]+foo.count[62] << std::endl;
    values << "Wooden Doors: " << foo.count[64]/2 << std::endl;
    values << "Iron Doors: " << foo.count[71] << std::endl;
    values << "Signs: " << foo.count[63]+foo.count[68] << std::endl;
    values << "Ladder: " << foo.count[65] << std::endl;
    values << "Railtracks: " << foo.count[66] << std::endl;
    values << "Wooden Stairs: " << foo.count[53] << std::endl;
    values << "Rock Stairs: " << foo.count[67] << std::endl;
    values << "Lever: " << foo.count[69] << std::endl;
    values << "Buttons: " << foo.count[77] << std::endl;
    values << "Pressure Plates: " << foo.count[70]+foo.count[72] << std::endl;
    values << "Redstone Powder: " << foo.count[55] << std::endl;
    values << "Redstone Torches: " << foo.count[75]+foo.count[76] << std::endl;

    values << std::endl;



  values.close();
  }else std::cout << "fatal write to txt" << std::endl;
/*
  for (std::list<render>::iterator it=renderblocks.begin();it!=renderblocks.end();++it){
  for(int x = 0;x < 16;x++){
      for(int y = 0;y < 16;y++){
        std::cout << " " << (int)it->data[x+y*16];
      }}
      int p;
      std::cin >> p;
    }
*/

   // return 0;
}

int main(){

  std::string flag = "";

  std::cout << "Cartography rewritten for linux by Firemark [pozdrawiam halp]" << std::endl;

  while (CWorld>5||CWorld<1){
    std::cout << "select world:[1,2,3,4,5]" << std::endl << ":";
    std::cin >> CWorld;
  }

    slide = -1;
    std::cout << "Select flags:" << std::endl
    << "W - water; C - cave mode" << std::endl
    << "R - rotate; F - flip  " << std::endl
    << "D - day; d -day/night; N - night " << std::endl
    << "Hc - heightcolor; Hg - heightgray; Or - Ore " << std::endl
    << "Ob - oblique; Oa - oblique angle" << std::endl
    << "E - exclude " << std::endl << ":";

    std::cin >> flag;

    if ( flag.find("W") != std::string::npos )
      water = true;
    else
      water = false;

    if ( flag.find("C") != std::string::npos )
      cave = true;
    else
      cave = false;

    if ( flag.find("R") != std::string::npos )
      Rotate = true;
    else
      Rotate = false;

    if ( flag.find("F") != std::string::npos )
      flip = true;
    else
      flip = false;

    if ( flag.find("D") != std::string::npos )
      daynight = 0;
    if ( flag.find("d") != std::string::npos )
      daynight = 1;
    if ( flag.find("n") != std::string::npos )
      daynight = 2;

    if ( flag.find("Hc") != std::string::npos )
      slide = -3;
    if ( flag.find("Hg") != std::string::npos )
      slide = -4;
    if ( flag.find("Or") != std::string::npos )
      slide = -5;
    if ( flag.find("Ob") != std::string::npos )
      slide = 69;//yeah funny
    if ( flag.find("Oa") != std::string::npos )
      slide = 70;
    if ( flag.find("E") != std::string::npos ){
        std::cout << "Write number to exclude [0-128]" << std::endl << ":";
        std::cin >> exclude;
    }



  Dostuff();

  return 0;
};


