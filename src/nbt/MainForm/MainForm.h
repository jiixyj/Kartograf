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
  void populateSceneItem(int i, int j);
  void scale();

 signals:
  void startPopulatingScene();
  void scaleSig();

 protected:
  void mousePressEvent(QMouseEvent* event);
  void mouseDoubleClickEvent(QMouseEvent* event);

 private:
  QGraphicsScene* scene_;
  nbt* bf_;

  int scale_;

  MainForm(const MainForm&);
  MainForm& operator=(const MainForm&);
};

#endif  // SRC_MAINFORM_MAINFORM_H_
