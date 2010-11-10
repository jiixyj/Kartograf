#include "MainGUI.moc"

#include <cstdlib>

#include "../assemble.h"

MainGUI::MainGUI(std::string world_string) {
  int world = std::atoi(world_string.c_str());
  bf = world ? new nbt(world) : new nbt(world_string);
  std::cout << bf->string();
  bf->setSettings(getSettings());
  mf = new MainForm(&scene, bf);
  mf->show();

  QFuture<void>(QtConcurrent::run(mf, &MainForm::populateScene));
}
