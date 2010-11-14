#include "MainGUI.moc"

#include <cstdlib>
#include <QPushButton>

#include "../assemble.h"

MainGUI::MainGUI(std::string world_string) {
  QGroupBox *groupBox = new QGroupBox("select world");
  QRadioButton *radio1 = new QRadioButton(tr("World 1"));
  QRadioButton *radio2 = new QRadioButton(tr("World 2"));
  QRadioButton *radio3 = new QRadioButton(tr("World 3"));
  QRadioButton *radio4 = new QRadioButton(tr("World 4"));
  QRadioButton *radio5 = new QRadioButton(tr("World 5"));
  // radio1->setChecked(true);
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->addWidget(radio1);
  vbox->addWidget(radio2);
  vbox->addWidget(radio3);
  vbox->addWidget(radio4);
  vbox->addWidget(radio5);
  vbox->addStretch(1);
  groupBox->setLayout(vbox);

  int world = std::atoi(world_string.c_str());
  bf = world ? new nbt(world) : new nbt(world_string);
  std::cout << bf->string();
  bf->setSettings(getSettings());

  QBoxLayout* global = new QHBoxLayout(this);

  QBoxLayout* left_side = new QVBoxLayout;
  left_side->addWidget(groupBox);

  start_button = new QPushButton("Start rendering", this);
  connect(start_button, SIGNAL(clicked()), this, SLOT(toggle_rendering()));
  left_side->addWidget(start_button);
  connect(&watcher, SIGNAL(finished()), this, SLOT(handle_finished()));

  global->addLayout(left_side);

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
