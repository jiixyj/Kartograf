#include <iostream>
#include <QtGui>
#include <QApplication>
#include <limits>
#include <cmath>
#include <bitset>

#include "./nbt.h"
#include "./MainForm/MainForm.h"

int main(int ac, char* av[]) {
  QApplication app(ac, av);
  if (ac != 2) {
    std::cerr << "Usage: ./nbtparse [filename | world number]" << std::endl;
    exit(1);
  }
  int world = atoi(av[1]);
  nbt bf;
  if (world == 0) {
    bf = nbt(av[1]);
  } else {
    bf = nbt(world);
  }
  std::cout << bf.string();
  Settings set;
  set.topview = true;
  set.heightmap = false;
  set.color = false;
  set.shadow = true;
  set.relief = true;
  bf.setSettings(set);
  QGraphicsScene scene;
  MainForm label(&scene, &bf);
  label.show();

  QObject::connect(&label, SIGNAL(startPopulatingScene()), &label, SLOT(populateScene()));

  QFuture<void>(QtConcurrent::run(&label, &MainForm::getGoing));

  return app.exec();
}
