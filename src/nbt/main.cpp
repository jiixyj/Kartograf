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
  set.topview = false;
  set.oblique = true;
  set.heightmap = false;
  set.color = false;
  set.shadow = true;
  set.relief = true;
  set.sun_direction = 7;
  set.rotate = 0;
  bf->setSettings(set);
  QGraphicsScene scene;
  MainForm label(&scene, bf);
  label.show();

  QFuture<void>(QtConcurrent::run(&label, &MainForm::populateScene));

  return app.exec();
}
