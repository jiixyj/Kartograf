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

  start_button = new QPushButton("Start rendering", this);
  connect(start_button, SIGNAL(clicked()), this, SLOT(toggle_rendering()));
  global->addWidget(start_button);
  connect(&watcher, SIGNAL(finished()), this, SLOT(handle_finished()));

  mf = new MainForm(&scene, bf);
  global->addWidget(mf);
}

void MainGUI::handle_finished() {
  start_button->setText("Start rendering");
  start_button->setEnabled(true);
}

void MainGUI::toggle_rendering() {
  if (start_button->text() == "Start rendering") {
    start_button->setText("Abort rendering");
    worker = QFuture<void>(QtConcurrent::run(mf, &MainForm::populateScene));
    watcher.setFuture(worker);
  } else {
    mf->StopPopulateScene();
    start_button->setEnabled(false);
  }
}
