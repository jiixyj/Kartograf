#include <QtGui>
#include <iostream>
#include <string>

#include "./nbt.h"
#include "./MainForm/MainForm.h"

int main(int ac, char* av[]) {
  QApplication app(ac, av);
  if (ac != 2) {
    std::cerr << "Usage: ./nbtparse [filename | world number]" << std::endl;
    exit(1);
  }
  int world = atoi(av[1]);
  nbt* bf;
  if (world == 0) {
    bf = new nbt(av[1]);
  } else {
    bf = new nbt(world);
  }
  std::cout << bf->string();
  Settings set;
  set.topview = true;
  set.oblique = false;
  set.heightmap = false;
  set.color = false;
  set.shadow_strength = 60;
  set.shadow_quality = true;
  set.relief_strength = 10;
  set.sun_direction = 7;
  set.rotate = 0;
  if (set.shadow_strength != 0) {
    set.shadow = true;
  } else {
    set.shadow = false;
  }
  if (set.relief_strength != 0) {
    set.relief = true;
  } else {
    set.relief = false;
  }
  bf->setSettings(set);
  QGraphicsScene scene;
  MainForm label(&scene, bf);
  label.show();

  QFuture<void>(QtConcurrent::run(&label, &MainForm::populateScene));

  return app.exec();
}
