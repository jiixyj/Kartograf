/* See LICENSE file for copyright and license details. */
#ifndef SRC_NBT_MAINFORM_MAINGUI_H_
#define SRC_NBT_MAINFORM_MAINGUI_H_

#include <boost/thread.hpp>

#include "./MainForm.h"
#include "../settings.h"

class MainGUI : public QWidget {
  Q_OBJECT

 public:
  MainGUI();

 public slots:

 signals:
  void disable_shadow_elements_signal(bool);
  void save_image_with_filename_signal(QString);
  void toggle_rendering_signal(bool);

 protected:

 private slots:
  void set_current_world(int value);
  void set_render_mode(int value);
  void set_relief_strength(int value);
  void set_shadow_strength(int value);
  void set_sun_direction(int value);
  void set_rotate(int value);
  void set_night_mode(int value);
  void set_shadow_quality(int value);
  void set_heightmap(bool value);
  void set_heightmap_grey(bool value);
  void set_heightmap_color(bool value);
  void disable_shadow_elements(bool value);

  void set_new_world();
  void load_custom_world();
  void toggle_rendering(bool is_world_bad);
  void handle_finished();
  void save_image();
  void save_image_with_filename(QString filename);

 private:
  int current_world;
  QLineEdit* custom_world;
  QGroupBox* heightmapBox;

  Renderer* bf;
  QGraphicsScene* scene;
  MainForm* mf;
  Settings set;

  void save_image_dialog();

  QPushButton* start_button;
  void start_helper();

  boost::thread worker_thread;
  boost::thread waiter_thread;
  void start_populate_scene_thread();
  void new_bf();

  MainGUI(const MainGUI&);
  MainGUI& operator=(const MainGUI&);
};

#endif  // SRC_NBT_MAINFORM_MAINFORM_H_
