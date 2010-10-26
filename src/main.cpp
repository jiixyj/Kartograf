#include <QtGui>
#include <iostream>
#include <string>

#include "./MainForm/MainForm.h"
#include "./assemble.h"

int main(int ac, char* av[]) {
  QApplication app(ac, av);
  if (ac != 2) {
    std::cerr << "Usage: ./nbtparse [filename | world number]" << std::endl;
    return 1;
  }
  int world = atoi(av[1]);
  nbt bf = world ? nbt(world) : nbt(av[1]);
  std::cout << bf.string();
  bf.setSettings(getSettings());
  QGraphicsScene scene;
  MainForm label(&scene, &bf);
  label.show();

  QFuture<void>(QtConcurrent::run(&label, &MainForm::populateScene));

  return app.exec();
}
