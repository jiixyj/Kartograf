#include "MainForm.moc"

#include <QtGui>
#include <iostream>
#include <sstream>
#include <tbb/tbb.h>
#include <png.h>

MainForm::MainForm(QGraphicsScene* img, nbt* bf, QWidget* parent_)
                 : QGraphicsView(img, parent_), scene_(), bf_(bf), scale_(1),
                   images() {
  connect(this, SIGNAL(scaleSig()), this, SLOT(scale()));
  connect(this, SIGNAL(renderNewImage()), this, SLOT(populateSceneItem()));
  connect(this, SIGNAL(saveToFileSignal()), this, SLOT(saveToFile()));
  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

class ApplyFoo {
  MainForm* mainform_;
  int i_;
  tbb::atomic<int>* index_;
 public:
  void operator()( const tbb::blocked_range<int32_t>& r ) const {
    for(int32_t j=r.begin(); j!=r.end(); ++j) {
      bool result = false;
      QPoint bp = mainform_->projectCoords(QPoint(j, i_),
                                        (4 - mainform_->bf_->set().rotate) % 4);
      const Image<uint8_t>& image = mainform_->bf_-> getImage(bp.x(),
                                                              bp.y(), &result);
      if (!result) {
        continue;
      }
      *index_ += 1;
      mainform_->images.push(MainForm::image_coords(image, QPoint(bp.x(),
                                                                  bp.y())));
      mainform_->renderNewImageEmitter();
    }
  }
  ApplyFoo(MainForm* mainform, int i, tbb::atomic<int>* index)
          : mainform_(mainform), i_(i), index_(index) {}
  /* just for the compiler */
  ApplyFoo(const ApplyFoo& rhs)
          : mainform_(rhs.mainform_), i_(rhs.i_), index_(rhs.index_) {}
 private:
  ApplyFoo& operator=(const ApplyFoo&);
};


void MainForm::populateScene() {
  tbb::atomic<int> index;
  index = 0;
  QPoint min(bf_->xPos_min(), bf_->zPos_min());
  min = projectCoords(min, bf_->set().rotate);
  QPoint max(bf_->xPos_max(), bf_->zPos_max());
  max = projectCoords(max, bf_->set().rotate);
  min_norm = QPoint(std::min(min.x(), max.x()),
                    std::min(min.y(), max.y()));
  max_norm = QPoint(std::max(min.x(), max.x()),
                    std::max(min.y(), max.y()));

  for (int i = min_norm.y(); i <= max_norm.y(); ++i) {
    tbb::parallel_for(tbb::blocked_range<int32_t>(min_norm.x(),
                                                  max_norm.x() + 1),
                                                  ApplyFoo(this, i, &index));
    if (index > 10000) {
      std::cerr << "cache cleared!" << std::endl;
      index = 0;
      bf_->clearCache();
    }
  }
  bf_->clearCache();
  emit saveToFileSignal();
}

void MainForm::renderNewImageEmitter() {
  emit renderNewImage();
}

void MainForm::saveToFile() {
  QImage image(scene()->sceneRect().toRect().size(), QImage::Format_ARGB32);
  image.fill(0);
  QPainter painter(&image);
  scene()->render(&painter, painter.viewport(), scene()->sceneRect());
  image.save("image.png");
  fprintf(stderr, "image saved!\n");
  // exit(1);
}

QPoint MainForm::projectCoords(QPoint p, int phi) {
  return projectCoords(p.x(), p.y(), phi);
}

QPoint MainForm::projectCoords(int _x, int _y, int phi) {
  if (phi == 0) return QPoint(_x, _y);
  int cos_phi = phi % 2 - 1;
  if (!phi) cos_phi = 1;
  int sin_phi = phi % 2;
  if (phi == 3) sin_phi = -1;
  QPoint ret;
  ret.setX(_x * cos_phi - _y * sin_phi);
  ret.setY(_x * sin_phi + _y * cos_phi);
  return ret;
}

void MainForm::populateSceneItem() {
  MainForm::image_coords img_coor;
  if (images.try_pop(img_coor)) {
    QImage img(&(img_coor.first.data[0]),
               img_coor.first.cols,
               img_coor.first.rows,
               QImage::Format_ARGB32);
    QGraphicsPixmapItem* pi = scene()->addPixmap(QPixmap::fromImage(img));
    pi->setFlag(QGraphicsItem::ItemIsMovable, false);
    pi->setFlag(QGraphicsItem::ItemIsSelectable, false);
    QPoint projected = projectCoords(16 * img_coor.second.x(),
                                     16 * img_coor.second.y(),
                                     bf_->set().rotate);
    pi->setPos(projected);
    if (bf_->set().rotate == 0) {
      pi->setZValue(img_coor.second.y());
    } else if (bf_->set().rotate == 1) {
      pi->setZValue(img_coor.second.x());
    } else if (bf_->set().rotate == 2) {
      pi->setZValue(-img_coor.second.y());
    } else if (bf_->set().rotate == 3) {
      pi->setZValue(-img_coor.second.x());
    }
  } else {
    std::cerr << "must not happen!" << std::endl;
    exit(1);
  }
}

void MainForm::scale() {
  if (scale_ <= 0) {
    setTransform(QTransform().scale(1.0 / pow(2.0, abs(scale_ - 1)),
                                    1.0 / pow(2.0, abs(scale_ - 1))));
  } else {
    setTransform(QTransform().scale(scale_, scale_));
  }
}

void MainForm::mousePressEvent(QMouseEvent* mevent) {
  switch (mevent->button()) {
    case Qt::LeftButton:
      ++scale_;
      emit scaleSig();
      break;
    case Qt::MidButton:
      break;
    case Qt::RightButton:
      --scale_;
      emit scaleSig();
      break;
    default:
      break;
  }
  return;
}

void MainForm::mouseDoubleClickEvent(QMouseEvent* mevent) {
  mousePressEvent(mevent);
  return;
}
