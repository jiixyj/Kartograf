#include "MainGUI.moc"

#include <cstdlib>
#include <QPushButton>

#include "../assemble.h"

MainGUI::MainGUI(std::string world_string)
          : bf(NULL) {
  QGroupBox *groupBox = new QGroupBox("select world");
  radio1 = new QRadioButton(tr("World 1"));
  radio2 = new QRadioButton(tr("World 2"));
  radio3 = new QRadioButton(tr("World 3"));
  radio4 = new QRadioButton(tr("World 4"));
  radio5 = new QRadioButton(tr("World 5"));
  radio1->setEnabled(nbt::exist_world(1));
  radio2->setEnabled(nbt::exist_world(2));
  radio3->setEnabled(nbt::exist_world(3));
  radio4->setEnabled(nbt::exist_world(4));
  radio5->setEnabled(nbt::exist_world(5));
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->addWidget(radio1);
  vbox->addWidget(radio2);
  vbox->addWidget(radio3);
  vbox->addWidget(radio4);
  vbox->addWidget(radio5);
  vbox->addStretch(1);
  groupBox->setLayout(vbox);

  QBoxLayout* global = new QHBoxLayout(this);

  QBoxLayout* left_side = new QVBoxLayout;
  left_side->addWidget(groupBox);

  start_button = new QPushButton("Start rendering", this);
  left_side->addWidget(start_button);

  global->addLayout(left_side);

  mf = new MainForm(&scene, bf);
  global->addWidget(mf);

  connect(start_button, SIGNAL(clicked()), this, SLOT(set_new_world()));
  connect(&new_world_setup_watcher, SIGNAL(finished()), this, SLOT(toggle_rendering()));
  connect(&watcher, SIGNAL(finished()), this, SLOT(handle_finished()));
}

int MainGUI::current_world() {
  if (radio1->isChecked()) {
    return 1;
  } else if (radio2->isChecked()) {
    return 2;
  } else if (radio3->isChecked()) {
    return 3;
  } else if (radio4->isChecked()) {
    return 4;
  } else if (radio5->isChecked()) {
    return 5;
  } else {
    return 0;
  }
}

void MainGUI::new_bf() {
  bf = new nbt(current_world());
}

void MainGUI::set_new_world() {
  if (start_button->text() == "Start rendering") {
    if (bf) {
      delete bf;
      bf = NULL;
    }
    if (!current_world()) return;
    start_button->setText("Loading world...");
    start_button->setEnabled(false);
    new_world_setup = QFuture<void>(QtConcurrent::run(this, &MainGUI::new_bf));
    new_world_setup_watcher.setFuture(new_world_setup);
  } else {
    mf->StopPopulateScene();
    start_button->setEnabled(false);
  }
}

void MainGUI::toggle_rendering() {
  std::cout << bf->string();
  bf->setSettings(getSettings());
  mf->set_nbt(bf);
  scene.clear();

  start_button->setText("Abort rendering");
  start_button->setEnabled(true);
  worker = QFuture<void>(QtConcurrent::run(mf, &MainForm::populateScene));
  watcher.setFuture(worker);
}

void MainGUI::handle_finished() {
  start_button->setText("Start rendering");
  start_button->setEnabled(true);
}
