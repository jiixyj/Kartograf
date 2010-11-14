#ifndef SRC_NBT_MAINFORM_MAINGUI_H_
#define SRC_NBT_MAINFORM_MAINGUI_H_

#include "./MainForm.h"

class MainGUI : public QWidget {
  Q_OBJECT

 public:
  MainGUI(std::string world_string);

 public slots:
  void set_new_world();
  void toggle_rendering();
  void handle_finished();

 signals:

 protected:

 private:
  QRadioButton *radio1;
  QRadioButton *radio2;
  QRadioButton *radio3;
  QRadioButton *radio4;
  QRadioButton *radio5;
  int current_world();

  nbt* bf;
  MainForm* mf;
  QPushButton* start_button;
  QGraphicsScene scene;
  void start_helper();
  QFuture<void> worker;
  QFutureWatcher<void> watcher;

  QFuture<void> new_world_setup;
  QFutureWatcher<void> new_world_setup_watcher;
  void new_bf();
};

#endif  // SRC_NBT_MAINFORM_MAINFORM_H_
