/* See LICENSE file for copyright and license details. */
#ifndef SRC_NBT_MAINFORM_MAINFORM_H_
#define SRC_NBT_MAINFORM_MAINFORM_H_

#include <QtGui>
#define TBB_DEPRECATED true
#include <tbb/concurrent_queue.h>

#include "../image.h"

class Renderer;

class MainForm : public QGraphicsView {
  Q_OBJECT

 public:
  MainForm(QGraphicsScene* img, Renderer* bf, QWidget* parent = NULL);
  void set_nbt(Renderer* bf) {
    bf_ = bf;
  }
  void reset_view(QGraphicsScene* new_scene) {
    setScene(new_scene);
  }

 public slots:
  void populateScene();
  void StopPopulateScene();
  void populateSceneItem();
  void scale();
  void renderNewImageEmitter();

 signals:
  void scaleSig();
  void renderNewImage();

 protected:
  void mousePressEvent(QMouseEvent* event);
  void mouseDoubleClickEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);

 private:
  friend class ApplyFooQT;

  Renderer* bf_;
  int16_t scale_;
  typedef QPair<Image<uint8_t>, QPoint> image_coords;
  tbb::concurrent_queue<image_coords> images;

  bool stop;

  MainForm(const MainForm&);
  MainForm& operator=(const MainForm&);
};

#endif  // SRC_NBT_MAINFORM_MAINFORM_H_
