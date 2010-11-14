#ifndef SRC_NBT_MAINFORM_MAINGUI_H_
#define SRC_NBT_MAINFORM_MAINGUI_H_

#include "./MainForm.h"

class MainGUI : public QWidget {
  Q_OBJECT

 public:
  MainGUI(std::string world_string);

 public slots:
  void toggle_rendering();
  void handle_finished();

 signals:

 protected:

 private:
  nbt* bf;
  MainForm* mf;
  QPushButton* start_button;
  QGraphicsScene scene;
  void start_helper();
  QFuture<void> worker;
  QFutureWatcher<void> watcher;
};

#endif  // SRC_NBT_MAINFORM_MAINFORM_H_
