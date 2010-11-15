#ifndef SRC_NBT_MAINFORM_MAINGUI_H_
#define SRC_NBT_MAINFORM_MAINGUI_H_

#include "./MainForm.h"
#include "../settings.h"

class MainGUI : public QWidget {
  Q_OBJECT

 public:
  MainGUI();

 public slots:

 signals:

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

  void set_new_world();
  void load_custom_world();
  void toggle_rendering();
  void handle_finished();

 private:
  int current_world;
  QLineEdit* custom_world;

  nbt* bf;
  QGraphicsScene* scene;
  MainForm* mf;
  Settings set;

  QPushButton* start_button;
  void start_helper();
  QFuture<void> worker;
  QFutureWatcher<void> watcher;

  QFuture<void> new_world_setup;
  QFutureWatcher<void> new_world_setup_watcher;
  void new_bf();

  MainGUI(const MainGUI&);
  MainGUI& operator=(const MainGUI&);
};

#endif  // SRC_NBT_MAINFORM_MAINFORM_H_
