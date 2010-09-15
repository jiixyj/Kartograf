#ifndef SRC_NBT_MAINFORM_MAINFORM_H_
#define SRC_NBT_MAINFORM_MAINFORM_H_

#include <QtGui>
#include <tbb/concurrent_queue.h>

#include "../nbt.h"

class MainForm : public QGraphicsView {
  Q_OBJECT

 public:
  MainForm(QGraphicsScene* img, nbt* bf, QWidget* parent = NULL);

 public slots:
  void populateScene();
  void populateSceneItem();
  void scale();
  void saveToFile();

 signals:
  void scaleSig();
  void renderNewImage();
  void saveToFileSignal();

 protected:
  void mousePressEvent(QMouseEvent* event);
  void mouseDoubleClickEvent(QMouseEvent* event);

 private:
  QGraphicsScene* scene_;
  nbt* bf_;

  int scale_;

  tbb::strict_ppl::concurrent_queue<QImage> images;
  tbb::strict_ppl::concurrent_queue<QPair<int, int> > coords;

  MainForm(const MainForm&);
  MainForm& operator=(const MainForm&);
};

#endif  // SRC_NBT_MAINFORM_MAINFORM_H_
