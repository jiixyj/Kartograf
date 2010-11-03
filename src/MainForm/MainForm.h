#ifndef SRC_NBT_MAINFORM_MAINFORM_H_
#define SRC_NBT_MAINFORM_MAINFORM_H_

#include <QtGui>
#include <tbb/concurrent_queue.h>

#include "../image.h"

class nbt;

class MainForm : public QGraphicsView {
  Q_OBJECT

 public:
  MainForm(QGraphicsScene* img, nbt* bf, QWidget* parent = NULL);

 public slots:
  void populateScene();
  void populateSceneItem();
  void scale();
  void saveToFile();
  void renderNewImageEmitter();

 signals:
  void scaleSig();
  void renderNewImage();
  void saveToFileSignal();

 protected:
  void mousePressEvent(QMouseEvent* event);
  void mouseDoubleClickEvent(QMouseEvent* event);

 private:
  friend class ApplyFooQT;

  QGraphicsScene* scene_;
  nbt* bf_;
  int16_t scale_;
  typedef QPair<Image<uint8_t>, QPoint> image_coords;
  // tbb::strict_ppl::concurrent_queue<image_coords> images;
  tbb::concurrent_queue<image_coords> images;

  MainForm(const MainForm&);
  MainForm& operator=(const MainForm&);
};

#endif  // SRC_NBT_MAINFORM_MAINFORM_H_
