#include <iostream>
#include "./MainForm/MainGUI.h"

int main(int ac, char* av[]) {
  QApplication app(ac, av);
  MainGUI gui;
  gui.show();
  return app.exec();
}
