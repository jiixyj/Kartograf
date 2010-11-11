#include "MainGUI.moc"

#include <cstdlib>
#include <QPushButton>

#include "../assemble.h"

MainGUI::MainGUI(std::string world_string) {
  int world = std::atoi(world_string.c_str());
  bf = world ? new nbt(world) : new nbt(world_string);
  std::cout << bf->string();
  bf->setSettings(getSettings());

  QBoxLayout* global = new QHBoxLayout(this);

  QPushButton* start_button = new QPushButton("Start Rendering", this);
  connect(start_button, SIGNAL(clicked()), this, SLOT(start_rendering()));
  global->addWidget(start_button);

  mf = new MainForm(&scene, bf);
  global->addWidget(mf);
}

void MainGUI::start_rendering() {
  QFuture<void>(QtConcurrent::run(mf, &MainForm::populateScene));
}
