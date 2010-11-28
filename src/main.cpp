/* See LICENSE file for copyright and license details. */
#include <iostream>
#include "./MainForm/MainGUI.h"

int main(int ac, char* av[]) {
  QApplication app(ac, av);
  MainGUI gui;
  gui.show();
  return app.exec();
}
