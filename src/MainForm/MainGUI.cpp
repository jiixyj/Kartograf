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

  QGroupBox *lightBox = new QGroupBox("sun direction");
  QRadioButton* light0 = new QRadioButton();
  QRadioButton* light1 = new QRadioButton();
  QRadioButton* light2 = new QRadioButton();
  QRadioButton* light3 = new QRadioButton();
  QRadioButton* light4 = new QRadioButton();
  QRadioButton* light5 = new QRadioButton();
  QRadioButton* light6 = new QRadioButton();
  QRadioButton* light7 = new QRadioButton();
  QGridLayout *light_grid = new QGridLayout;
  light_grid->addWidget(light0, 0, 0);
  light_grid->addWidget(light1, 1, 0);
  light_grid->addWidget(light2, 2, 0);
  light_grid->addWidget(light3, 2, 1);
  light_grid->addWidget(light4, 2, 2);
  light_grid->addWidget(light5, 1, 2);
  light_grid->addWidget(light6, 0, 2);
  light_grid->addWidget(light7, 0, 1);
  lightBox->setLayout(light_grid);
  QSignalMapper* light_mapper = new QSignalMapper(this);
  connect(light0, SIGNAL(clicked()), light_mapper, SLOT(map()));
  connect(light1, SIGNAL(clicked()), light_mapper, SLOT(map()));
  connect(light2, SIGNAL(clicked()), light_mapper, SLOT(map()));
  connect(light3, SIGNAL(clicked()), light_mapper, SLOT(map()));
  connect(light4, SIGNAL(clicked()), light_mapper, SLOT(map()));
  connect(light5, SIGNAL(clicked()), light_mapper, SLOT(map()));
  connect(light6, SIGNAL(clicked()), light_mapper, SLOT(map()));
  connect(light7, SIGNAL(clicked()), light_mapper, SLOT(map()));
  light_mapper->setMapping(light0, 0);
  light_mapper->setMapping(light1, 1);
  light_mapper->setMapping(light2, 2);
  light_mapper->setMapping(light3, 3);
  light_mapper->setMapping(light4, 4);
  light_mapper->setMapping(light5, 5);
  light_mapper->setMapping(light6, 6);
  light_mapper->setMapping(light7, 7);
  connect(light_mapper, SIGNAL(mapped(int)), this, SLOT(set_sun_direction(int)));
  light0->click();

  QGroupBox *rotateBox = new QGroupBox("rotation");
  QRadioButton* rotate0 = new QRadioButton();
  QRadioButton* rotate1 = new QRadioButton();
  QRadioButton* rotate2 = new QRadioButton();
  QRadioButton* rotate3 = new QRadioButton();
  QGridLayout *rotate_grid = new QGridLayout;
  rotate_grid->addWidget(rotate0, 0, 1);
  rotate_grid->addWidget(rotate1, 1, 0);
  rotate_grid->addWidget(rotate2, 2, 1);
  rotate_grid->addWidget(rotate3, 1, 2);
  rotateBox->setLayout(rotate_grid);
  QSignalMapper* rotate_mapper = new QSignalMapper(this);
  connect(rotate0, SIGNAL(clicked()), rotate_mapper, SLOT(map()));
  connect(rotate1, SIGNAL(clicked()), rotate_mapper, SLOT(map()));
  connect(rotate2, SIGNAL(clicked()), rotate_mapper, SLOT(map()));
  connect(rotate3, SIGNAL(clicked()), rotate_mapper, SLOT(map()));
  rotate_mapper->setMapping(rotate0, 1);
  rotate_mapper->setMapping(rotate1, 0);
  rotate_mapper->setMapping(rotate2, 3);
  rotate_mapper->setMapping(rotate3, 2);
  connect(rotate_mapper, SIGNAL(mapped(int)), this, SLOT(set_rotate(int)));
  rotate0->click();

  QSpinBox* relief_strength = new QSpinBox(this);
  connect(relief_strength, SIGNAL(valueChanged(int)), this, SLOT(set_relief_strength(int)));
  relief_strength->setValue(10);
  QSpinBox* shadow_strength = new QSpinBox(this);
  connect(shadow_strength, SIGNAL(valueChanged(int)), this, SLOT(set_shadow_strength(int)));
  shadow_strength->setValue(60);
  QLabel* relief_label = new QLabel("relief strength:");
  QLabel* shadow_label = new QLabel("shadow strength:");

  QBoxLayout* global = new QHBoxLayout(this);
  QScrollArea* left_scroll_area = new QScrollArea;
  QBoxLayout* left_side = new QVBoxLayout;
  left_side->addWidget(groupBox);
  left_side->addWidget(renderBox);
  left_side->addWidget(relief_label);
  left_side->addWidget(relief_strength);
  left_side->addWidget(shadow_label);
  left_side->addWidget(shadow_strength);
  left_side->addWidget(lightBox);
  left_side->addWidget(rotateBox);
  left_side->addStretch(1);
  QWidget* left_widget = new QWidget;
  left_widget->setLayout(left_side);
  left_scroll_area->setWidget(left_widget);
  QVBoxLayout* left_side_all = new QVBoxLayout;
  left_side_all->addWidget(left_scroll_area);
  start_button = new QPushButton("Start rendering", this);
  left_side_all->addWidget(start_button);
  left_side_all->setContentsMargins(0, 0, 0, 0);
  QWidget* left_side_all_widget = new QWidget;
  left_side_all_widget->setLayout(left_side_all);
  left_side_all_widget->setFixedWidth(left_side_all_widget->sizeHint().width()
                                    + left_scroll_area->verticalScrollBar()
                                                      ->sizeHint().width() + 3);
  global->addWidget(left_side_all_widget);


  mf = new MainForm(&scene, bf);
  global->addWidget(mf);

  connect(start_button, SIGNAL(clicked()), this, SLOT(set_new_world()));
  connect(&new_world_setup_watcher, SIGNAL(finished()), this, SLOT(toggle_rendering()));
  connect(&watcher, SIGNAL(finished()), this, SLOT(handle_finished()));
}

void MainGUI::set_relief_strength(int value) {
  set.relief = set.relief_strength = value;
}

void MainGUI::set_shadow_strength(int value) {
  set.shadow = set.shadow_strength = value;
}

void MainGUI::set_sun_direction(int value) {
  value = (value + 1) % 8;
  set.sun_direction = value;
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

void MainGUI::set_rotate(int value) {
  set.rotate = value;
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
