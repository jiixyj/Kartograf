#ifndef SRC_NBT_MAINFORM_MAINGUI_H_
#define SRC_NBT_MAINFORM_MAINGUI_H_

#include "./MainForm.h"

class MainGUI : public QWidget {
  Q_OBJECT

 public:
  MainGUI(std::string world_string);

 public slots:
  void start_rendering();

 signals:

 protected:

 private:
  nbt* bf;
  MainForm* mf;
  QGraphicsScene scene;
};

#endif  // SRC_NBT_MAINFORM_MAINFORM_H_
