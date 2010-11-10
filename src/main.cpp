#include <iostream>
#include "./MainForm/MainGUI.h"

int main(int ac, char* av[]) {
  QApplication app(ac, av);
  if (ac != 2) {
    std::cerr << "Usage: ./nbtparse [filename | world number]" << std::endl;
    return 1;
  }
  MainGUI gui(av[1]);
  gui.show();
  return app.exec();
}
