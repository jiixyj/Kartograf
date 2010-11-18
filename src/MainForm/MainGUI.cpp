#include "MainGUI.moc"

#include <cstdlib>
#include <QPushButton>

#include "../assemble.h"

MainGUI::MainGUI()
          : current_world(0),
            custom_world(new QLineEdit),
            bf(NULL),
            scene(new QGraphicsScene()),
            mf(new MainForm(scene, bf)),
            set(),
            start_button(new QPushButton("Start rendering", this)),
            worker(),
            watcher(),
            new_world_setup(),
            new_world_setup_watcher() {

  QGroupBox* groupBox = new QGroupBox("select world");
  QRadioButton* radio1 = new QRadioButton("World 1");
  QRadioButton* radio2 = new QRadioButton("World 2");
  QRadioButton* radio3 = new QRadioButton("World 3");
  QRadioButton* radio4 = new QRadioButton("World 4");
  QRadioButton* radio5 = new QRadioButton("World 5");
  QRadioButton* radio6 = new QRadioButton("custom");
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
  vbox->addWidget(radio6);
  groupBox->setLayout(vbox);
  custom_world->setMaximumWidth(groupBox->sizeHint().width());
  custom_world->setEnabled(false);
  vbox->addWidget(custom_world);
  QSignalMapper* world_mapper = new QSignalMapper(this);
  connect(radio1, SIGNAL(clicked()), world_mapper, SLOT(map()));
  connect(radio2, SIGNAL(clicked()), world_mapper, SLOT(map()));
  connect(radio3, SIGNAL(clicked()), world_mapper, SLOT(map()));
  connect(radio4, SIGNAL(clicked()), world_mapper, SLOT(map()));
  connect(radio5, SIGNAL(clicked()), world_mapper, SLOT(map()));
  connect(radio6, SIGNAL(clicked()), world_mapper, SLOT(map()));
  world_mapper->setMapping(radio1, 1);
  world_mapper->setMapping(radio2, 2);
  world_mapper->setMapping(radio3, 3);
  world_mapper->setMapping(radio4, 4);
  world_mapper->setMapping(radio5, 5);
  world_mapper->setMapping(radio6, 0);
  connect(world_mapper, SIGNAL(mapped(int)), this, SLOT(set_current_world(int)));
  connect(radio6, SIGNAL(toggled(bool)), custom_world, SLOT(setEnabled(bool)));
  connect(radio6, SIGNAL(clicked()), this, SLOT(load_custom_world()));

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

  QGroupBox* shadowBox = new QGroupBox("shadow quality");
  QRadioButton* shadow1 = new QRadioButton("normal");
  QRadioButton* shadow2 = new QRadioButton("high");
  QRadioButton* shadow3 = new QRadioButton("ultra");
  shadow1->setChecked(true);
  QVBoxLayout *vbox_shadow = new QVBoxLayout;
  vbox_shadow->addWidget(shadow1);
  vbox_shadow->addWidget(shadow2);
  vbox_shadow->addWidget(shadow3);
  shadowBox->setLayout(vbox_shadow);
  QSignalMapper* shadow_mapper = new QSignalMapper(this);
  connect(shadow1, SIGNAL(clicked()), shadow_mapper, SLOT(map()));
  connect(shadow2, SIGNAL(clicked()), shadow_mapper, SLOT(map()));
  connect(shadow3, SIGNAL(clicked()), shadow_mapper, SLOT(map()));
  shadow_mapper->setMapping(shadow1, 0);
  shadow_mapper->setMapping(shadow2, 1);
  shadow_mapper->setMapping(shadow3, 2);
  connect(shadow_mapper, SIGNAL(mapped(int)), this, SLOT(set_shadow_quality(int)));

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
  left_scroll_area->setFrameStyle(QFrame::NoFrame | QFrame::Plain);
  left_scroll_area->setLineWidth(0);
  QBoxLayout* left_side = new QVBoxLayout;
  left_side->addWidget(groupBox);
  left_side->addWidget(renderBox);
  left_side->addWidget(shadowBox);
  left_side->addWidget(lightBox);
  left_side->addWidget(shadow_label);
  left_side->addWidget(shadow_strength);
  left_side->addWidget(relief_label);
  left_side->addWidget(relief_strength);
  left_side->addWidget(rotateBox);

  QCheckBox* night_mode_box = new QCheckBox("night mode");
  left_side->addWidget(night_mode_box);
  connect(night_mode_box, SIGNAL(stateChanged(int)), this, SLOT(set_night_mode(int)));

  heightmapBox = new QGroupBox("heightmap");
  heightmapBox->setCheckable(true);
  heightmapBox->setChecked(false);
  QRadioButton* heightmap_box1 = new QRadioButton("grey");
  heightmap_box1->setChecked(true);
  QRadioButton* heightmap_box2 = new QRadioButton("color");
  QVBoxLayout *vbox_heightmap = new QVBoxLayout;
  vbox_heightmap->addWidget(heightmap_box1);
  vbox_heightmap->addWidget(heightmap_box2);
  heightmapBox->setLayout(vbox_heightmap);
  left_side->addWidget(heightmapBox);
  connect(heightmapBox, SIGNAL(toggled(bool)), this, SLOT(set_heightmap(bool)));
  connect(heightmap_box1, SIGNAL(toggled(bool)), this, SLOT(set_heightmap_grey(bool)));
  connect(heightmap_box2, SIGNAL(toggled(bool)), this, SLOT(set_heightmap_color(bool)));

  left_side->addStretch(1);
  QWidget* left_widget = new QWidget;
  left_widget->setLayout(left_side);
  left_scroll_area->setWidget(left_widget);
  QVBoxLayout* left_side_all = new QVBoxLayout;
  left_side_all->addWidget(left_scroll_area);
  left_side_all->addWidget(start_button);
  left_side_all->setContentsMargins(0, 0, 0, 0);
  QWidget* left_side_all_widget = new QWidget;
  left_side_all_widget->setLayout(left_side_all);
  left_side_all_widget->setFixedWidth(left_side_all_widget->sizeHint().width()
                                    + left_scroll_area->verticalScrollBar()
                                                      ->sizeHint().width() + 3);
  global->addWidget(left_side_all_widget);
  global->addWidget(mf);
  /* constraints */
  connect(render1, SIGNAL(toggled(bool)), heightmapBox, SLOT(setEnabled(bool)));
  connect(render1, SIGNAL(toggled(bool)), this, SLOT(disable_shadow_elements(bool)));
  connect(this, SIGNAL(disable_shadow_elements_signal(bool)), shadowBox, SLOT(setDisabled(bool)));
  connect(this, SIGNAL(disable_shadow_elements_signal(bool)), shadow_label, SLOT(setDisabled(bool)));
  connect(this, SIGNAL(disable_shadow_elements_signal(bool)), shadow_strength, SLOT(setDisabled(bool)));
  connect(this, SIGNAL(disable_shadow_elements_signal(bool)), lightBox, SLOT(setDisabled(bool)));
  connect(heightmapBox, SIGNAL(toggled(bool)), shadowBox, SLOT(setDisabled(bool)));
  connect(heightmapBox, SIGNAL(toggled(bool)), shadow_label, SLOT(setDisabled(bool)));
  connect(heightmapBox, SIGNAL(toggled(bool)), shadow_strength, SLOT(setDisabled(bool)));
  connect(heightmapBox, SIGNAL(toggled(bool)), lightBox, SLOT(setDisabled(bool)));

  connect(start_button, SIGNAL(clicked()), this, SLOT(set_new_world()));
  connect(&new_world_setup_watcher, SIGNAL(finished()), this, SLOT(toggle_rendering()));
  connect(&watcher, SIGNAL(finished()), this, SLOT(handle_finished()));
}

void MainGUI::disable_shadow_elements(bool value) {
  if (value && heightmapBox->isChecked())
    emit disable_shadow_elements_signal(true);
  else if (heightmapBox->isChecked())
    emit disable_shadow_elements_signal(false);
}

void MainGUI::set_relief_strength(int value) {
  set.relief = (set.relief_strength = value);
}

void MainGUI::set_shadow_strength(int value) {
  set.shadow = (set.shadow_strength = value);
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

void MainGUI::set_night_mode(int value) {
  set.nightmode = value;
}

void MainGUI::set_shadow_quality(int value) {
  if (value == 0) {
    set.shadow_quality = false;
    set.shadow_quality_ultra = false;
  } else if (value == 1) {
    set.shadow_quality = true;
    set.shadow_quality_ultra = false;
  } else if (value == 2) {
    set.shadow_quality = true;
    set.shadow_quality_ultra = true;
  }
}

void MainGUI::set_heightmap(bool value) {
  set.heightmap = value;
}

void MainGUI::set_heightmap_grey(bool value) {
  if (value)
    set.color = false;
}

void MainGUI::set_heightmap_color(bool value) {
  if (value)
    set.color = true;
}

void MainGUI::load_custom_world() {
  QString dir = QFileDialog::getExistingDirectory(this, "Open world",
                                                 QString(),
                                                 QFileDialog::ShowDirsOnly);
  if (dir != QString()) {
    custom_world->setText(dir);
  }
}


void MainGUI::set_current_world(int value) {
  current_world = value;
}

void MainGUI::new_bf() {
  if (!current_world) {
    bf = new nbt(custom_world->text().toStdString());
  } else {
    bf = new nbt(current_world);
  }
}

void MainGUI::set_new_world() {
  if (start_button->text() == "Start rendering") {
    if (bf) {
      delete bf;
      bf = NULL;
    }
    if (!current_world && custom_world->text() == QString()) {
      QMessageBox msgBox;
      msgBox.setText("Please select a world!");
      msgBox.setIcon(QMessageBox::Warning);
      msgBox.exec();
      return;
    }
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
  if (bf->bad_world) {
    handle_finished();
    QMessageBox msgBox;
    msgBox.setText("Invalid world! Check if you have selected the right folder.");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
    return;
  }
  std::cout << bf->string();
  bf->setSettings(set);
  mf->set_nbt(bf);
  delete scene;
  scene = new QGraphicsScene();
  mf->reset_view(scene);

  start_button->setText("Abort rendering");
  start_button->setEnabled(true);
  worker = QFuture<void>(QtConcurrent::run(mf, &MainForm::populateScene));
  watcher.setFuture(worker);
}

void MainGUI::handle_finished() {
  start_button->setText("Start rendering");
  start_button->setEnabled(true);
}
