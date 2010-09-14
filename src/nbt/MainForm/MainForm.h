#ifndef SRC_MAINFORM_MAINFORM_H_
#define SRC_MAINFORM_MAINFORM_H_

#include <QtGui>

#include "../nbt.h"

class MainForm : public QGraphicsView {
  Q_OBJECT

 public:
  MainForm(QGraphicsScene* img, nbt* bf, QWidget* parent = NULL);
  void getGoing();

 public slots:
  void populateScene();
  void populateSceneH(int i, int j);

 signals:
  void startPopulatingScene();

 protected:

 private:
  QGraphicsScene* scene_;
  nbt* bf_;

  MainForm(const MainForm&);
  MainForm& operator=(const MainForm&);
};

#endif  // SRC_MAINFORM_MAINFORM_H_
