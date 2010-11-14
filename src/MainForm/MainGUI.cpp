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
  groupBox->setLayout(vbox);

  QGroupBox* renderBox = new QGroupBox("render mode");
  QRadioButton* render1 = new QRadioButton("top down");
  QRadioButton* render2 = new QRadioButton("oblique");
  QRadioButton* render3 = new QRadioButton("isometric");
  render1->setChecked(true);
  QVBoxLayout *vbox_render = new QVBoxLayout;
  vbox_render->addWidget(render1);
  vbox_render->addWidget(render2);
  vbox_render->addWidget(render3);
  renderBox->setLayout(vbox_render);
  QSignalMapper* render_mapper = new QSignalMapper(this);
  connect(render1, SIGNAL(clicked()), render_mapper, SLOT(map()));
  connect(render2, SIGNAL(clicked()), render_mapper, SLOT(map()));
  connect(render3, SIGNAL(clicked()), render_mapper, SLOT(map()));
  render_mapper->setMapping(render1, 0);
  render_mapper->setMapping(render2, 1);
  render_mapper->setMapping(render3, 2);
  connect(render_mapper, SIGNAL(mapped(int)), this, SLOT(set_render_mode(int)));

  QBoxLayout* global = new QHBoxLayout(this);


  QBoxLayout* left_side = new QVBoxLayout;

  left_side->addWidget(groupBox);
  left_side->addWidget(renderBox);
  left_side->addStretch(1);
  start_button = new QPushButton("Start rendering", this);
  left_side->addWidget(start_button);
  global->addLayout(left_side);


  mf = new MainForm(&scene, bf);
  global->addWidget(mf);

  connect(start_button, SIGNAL(clicked()), this, SLOT(set_new_world()));
  connect(&new_world_setup_watcher, SIGNAL(finished()), this, SLOT(toggle_rendering()));
  connect(&watcher, SIGNAL(finished()), this, SLOT(handle_finished()));
}

void MainGUI::set_render_mode(int value) {
  if (value == 0) {
    set.topview = true;
    set.oblique = false;
    set.isometric = false;
  } else if (value == 1) {
    set.topview = false;
    set.oblique = true;
    set.isometric = false;
  } else if (value == 2) {
    set.topview = false;
    set.oblique = false;
    set.isometric = true;
  }
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
  bf->setSettings(set);
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
