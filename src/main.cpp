#include "./global.h"

#include <iostream>
#include <QtGui>

#include "./Level.h"

int cut = 0;
bool water = true;

void Dostuff(int CWorld) {
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
    cerr << "Tried dir: " << dir.canonicalPath() << endl;
    qFatal("Minecraft is not installed!");
  }

  QString name = "map" + QString::number(CWorld);

  cout << dir.canonicalPath() << " " << name << endl;
  std::list<render> renderblocks;

  int cc = 0;
  Level foo;

  std::cout << "2)Unzip and draw..." << std::endl;
  dir.setFilter(QDir::Files);
  QDirIterator it(dir, QDirIterator::Subdirectories);
  while (it.hasNext()) {
    const render temp = foo.LoadLevelFromFile(it.next().toAscii().data(), slide, water, cut);
    if (temp.isgood) {
      cc++;
      renderblocks.push_back(temp);
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
  for (std::list<render>::iterator pit = renderblocks.begin(); pit != renderblocks.end(); pit++) {
    //std::cout << "  " << it->x << " " << it->y;
    if (abs(pit->x) < 65536 && abs(pit->y) < 65536) {

      if (pit->x < minx)
        minx = pit->x;
      if (pit->y < miny)
        miny = pit->y;

      if (pit->x > maxx)
        maxx = pit->x;
      if (pit->y > maxy)
        maxy = pit->y;

    }

  }
  //std::cout << "\n\nMAXIMUM X = " << maxx << " MAXIMUM Y = " << maxy;
  //std::cout << "\n\nMINIMUM X = " << minx << " MINIMUM Y = " << miny;
  int imageheight, imagewidth;



  if (slide == 70) {
    int cxx = ((((maxx - minx)) + (maxy - miny))) * 0.5;
    int cyy = (((maxx - minx)) + (maxy - miny)) * 0.5;

    imageheight = (cxx) * 32 + 288;
    imagewidth = (cyy) * 32 + 288;

  } else {
    imageheight = (maxx - minx) * 16 + 144;
    imagewidth = (maxy - miny) * 16 + 144;
  }
  QImage MAP;
  if (slide == 70)
    MAP = QImage(imageheight, imagewidth + 128, QImage::Format_ARGB32_Premultiplied);
  else if (slide == 69)
    MAP = QImage(imageheight, imagewidth, QImage::Format_ARGB32_Premultiplied);
  else
    MAP = QImage(imageheight, imagewidth, QImage::Format_ARGB32_Premultiplied);

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
  for (std::list<render>::iterator rit = renderblocks.begin(); rit != renderblocks.end(); rit++) {

    //total++;
    int xb = rit->x;
    int yb = rit->y;

    int xx = (xb - minx) * 16;
    int yy = (yb - miny) * 16;

    if (slide == 70) {
      if (flip == 1)
        yy = abs((yb - miny) - (maxy - miny)) * 16;
      if (Rotate == 0)
        xx = abs((xb - minx) - (maxx - minx)) * 16;



      int temx = xx;
      int temy = yy;
      xx = imageheight / 2 + (temx) - (temy);
      yy = (temx) + (temy) + 128;

    }

    if (slide == 69) {

      if (flip == 1) {
        if (Rotate == 0) {
          //xx = (xb-minx)*16;
          yy = abs((yb - miny) - (maxy - miny)) * 16;
        } else {
          xx = (yb - miny) * 16;
          yy = abs((xb - minx) - (maxx - minx)) * 16;

        }
      } else {
        if (Rotate == 1) {
          xx = (yb - miny) * 16;
          yy = (xb - minx) * 16;
        }
      }
    }
    //xytt[(xx/16)+(yy/16)*100]++;
    //if(xytt[(xx/16)+(yy/16)*100] > 1)
    //myfile << "X:" << xb << " Y:" << yb << " " << "xx:" << xx << " yy:" << yy << "  " << xytt[(xx/16)+(yy/16)*100] << std::endl;
    //std::cout << xx << "_" << yy << " ";
    if (xx >= 0 && yy >= 0 && xx < 40000 && yy < 40000) {
      if (slide == 70) {


        for (int xo = 0; xo < 33; xo++) {
          for (int yo = 0; yo < 160; yo++) {
            int x = xo;
            int y = yo;
            MAP.setPixel(x + xx, y + yy - 128, Blend(QColor::fromRgba(MAP.pixel(x + xx, y + yy - 128)), QColor::fromRgba(rit->Q.pixel(x, y)), 128).rgba());
          }
        }


      } else {
        for (int xo = 0; xo < 16; xo++) {
          for (int yo = 0; yo < 16 + (slide > 68) * 128; yo++) {
            int x = xo;
            int y = yo;

            //MAP.SetPixel(x+xx,y+yy,sf::Color(125,255,0,255));

            if (slide == 69)
              MAP.setPixel(x + xx, y + yy, Blend(QColor::fromRgba(MAP.pixel(x + xx, y + yy)), QColor::fromRgba(rit->Q.pixel(x, y)), 128).rgba());
            else if (slide == 70)
              MAP.setPixel(x + xx, y + yy - 128, Blend(QColor::fromRgba(MAP.pixel(x + xx, y + yy - 128)), QColor::fromRgba(rit->Q.pixel(x, y)), 128).rgba());
            else
              MAP.setPixel(x + xx, y + yy, rit->Q.pixel(x, y));

          }
        }
      }
    }
  }



  QString txtname = name + ".txt";

  //std::cout << "\n\nERRORS: " << errors << "\nTOTAL: " << total;
  switch (slide) {
  case(-1):
    name.append("-normal");
    if (!water)name.append("-nowater");
    break;
  case(-3):
    name.append("-heightcolor");
    if (!water)name.append("-nowater");
    break;
  case(-4):
    name.append("-heightgray");
    if (!water)name.append("-nowater");
    break;
  case(-5):
    name.append("-ore");
    break;
  case(69):
    name.append("-oblique");
    if (!water)name.append("-nowater");
    break;
  case(70):
    name.append("-obliqueangle");
    if (!water)name.append("-nowater");
    break;
  case(64): {
    name.append("-slice-");
    name.append(QString::number(cut));
  }
  break;


  }
  //if(flip)
  //name.append("-flipped");
  //if(Rotate)
  //name.append("-rotated");

  if (slide == -1 || slide > 68) {
    if (flip)
      name.append("-flipped");
    if (Rotate)
      name.append("-rotated");
    if (cave)
      name.append("-cave");



    switch (daynight) {
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

  if (exclude != 0) {
    name.append("-only-");
    name.append(QString::number(exclude));

  }




  std::cout << "3)Save image" << std::endl;
  MAP.save(name + ".png");

  //delete MAP;

  //remove("level_in");
  renderblocks.clear();
  std::cout << "4)Save txt" << std::endl;


  QFile data(txtname);
  if (data.open(QFile::WriteOnly | QFile::Truncate)) {
    QTextStream values(&data);
    values << "Map surface is: " << cc * 16 * 16 << " square meters";
    values << endl;
    values << "Block amounts:";
    values << endl;
    values << "Air: " << foo.count[0] << endl;
    values << "Stone: " << foo.count[1] << endl;
    values << "Grass: " << foo.count[2] << endl;
    values << "Dirt: " << foo.count[3] << endl;
    values << "Snow: " << foo.count[78] << endl;
    values << endl;
    values << "Water: " << foo.count[8] + foo.count[9] << endl;
    values << "Ice: " << foo.count[79] << endl;
    values << "Lava: " <<  foo.count[10] + foo.count[11] << endl;
    values << endl;
    values << "Obisidian: " <<  foo.count[49] << endl;
    values << endl;
    values << "Trunk: " << foo.count[17] << endl;
    values << "Leaves: " << foo.count[18] << endl;
    values << "Wood: " << foo.count[5] << endl;
    values << "Cactus: " << foo.count[81] << endl;
    values << endl;
    values << "Sand: " << foo.count[12] << endl;
    values << "Gravel: " << foo.count[13] << endl;
    values << "Clay: " << foo.count[82] << endl;
    values << endl;
    values << "Gold Ore: " << foo.count[14] << endl;
    values << "Iron Ore: " << foo.count[15] << endl;
    values << "Coal Ore: " << foo.count[16] << endl;
    values << "Diamond Ore: " << foo.count[56] << endl;
    values << "Redstone: " << foo.count[73] + foo.count[74] << endl;
    values << endl;
    values << "Cobble: " << foo.count[4] << endl;
    values << "Glass: " << foo.count[20] << endl;
    values << "Cloth: " << foo.count[35] << endl;
    values << "Gold: " << foo.count[41] << endl;
    values << "Iron: " << foo.count[42] << endl;
    values << "Diamond: " << foo.count[57] << endl;
    values << endl;
    values << "Farmland: " << foo.count[60] << endl;
    values << "Crops: " << foo.count[59] << endl;
    values << "Reed: " << foo.count[83] << endl;
    values << "Torch: " << foo.count[50] << endl;
    values << "CraftTable: " << foo.count[58] << endl;
    values << "Chest: " << foo.count[54] << endl;
    values << "Furnace: " << foo.count[61] + foo.count[62] << endl;
    values << "Wooden Doors: " << foo.count[64] / 2 << endl;
    values << "Iron Doors: " << foo.count[71] << endl;
    values << "Signs: " << foo.count[63] + foo.count[68] << endl;
    values << "Ladder: " << foo.count[65] << endl;
    values << "Railtracks: " << foo.count[66] << endl;
    values << "Wooden Stairs: " << foo.count[53] << endl;
    values << "Rock Stairs: " << foo.count[67] << endl;
    values << "Lever: " << foo.count[69] << endl;
    values << "Buttons: " << foo.count[77] << endl;
    values << "Pressure Plates: " << foo.count[70] + foo.count[72] << endl;
    values << "Redstone Powder: " << foo.count[55] << endl;
    values << "Redstone Torches: " << foo.count[75] + foo.count[76] << endl;

  } else std::cout << "fatal write to txt" << std::endl;
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

int main(int ac, const char* av[]) {
  std::string flag = "";

  std::cout << "Cartography rewritten for linux by Firemark [pozdrawiam halp]" << std::endl;

  int CWorld = 0;
  while (CWorld > 5 || CWorld < 1) {
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

  if (flag.find("W") != std::string::npos)
    water = true;
  else
    water = false;

  if (flag.find("C") != std::string::npos)
    cave = true;
  else
    cave = false;

  if (flag.find("R") != std::string::npos)
    Rotate = true;
  else
    Rotate = false;

  if (flag.find("F") != std::string::npos)
    flip = true;
  else
    flip = false;

  if (flag.find("D") != std::string::npos)
    daynight = 0;
  if (flag.find("d") != std::string::npos)
    daynight = 1;
  if (flag.find("n") != std::string::npos)
    daynight = 2;

  if (flag.find("Hc") != std::string::npos)
    slide = -3;
  if (flag.find("Hg") != std::string::npos)
    slide = -4;
  if (flag.find("Or") != std::string::npos)
    slide = -5;
  if (flag.find("Ob") != std::string::npos)
    slide = 69;//yeah funny
  if (flag.find("Oa") != std::string::npos)
    slide = 70;
  if (flag.find("E") != std::string::npos) {
    std::cout << "Write number to exclude [0-128]" << std::endl << ":";
    std::cin >> exclude;
  }



  Dostuff(CWorld);

  return 0;
};
