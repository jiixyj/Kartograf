#include "MainForm.moc"

#include <QtGui>
#include <iostream>
#include <tbb/tbb.h>

MainForm::MainForm(QGraphicsScene* img, nbt* bf, QWidget* parent_)
                 : QGraphicsView(img, parent_), scene_(), bf_(bf), scale_(1),
                   images() {
  connect(this, SIGNAL(scaleSig()), this, SLOT(scale()));
  connect(this, SIGNAL(renderNewImage()), this, SLOT(populateSceneItem()));
  connect(this, SIGNAL(saveToFileSignal()), this, SLOT(saveToFile()));
  rotate(90 * bf_->set().rotate);
}

class ApplyFoo {
  MainForm* mainform_;
  int i_;
  tbb::atomic<int>* index_;
 public:
  void operator()( const tbb::blocked_range<int32_t>& r ) const {
    for(int32_t j=r.begin(); j!=r.end(); ++j) {
      bool result = false;
      const QImage& img = mainform_->bf_->getImage(j, i_, &result);
      if (!result) {
        continue;
      }
      *index_ += 1;
      mainform_->images.push(MainForm::image_coords(img, QPair<int, int>(j, i_)));
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
  for (int i = bf_->zPos_min(); i <= bf_->zPos_max(); ++i) {
    tbb::parallel_for(tbb::blocked_range<int32_t>(bf_->xPos_min(),
                                                  bf_->xPos_max() + 1),
                                                  ApplyFoo(this, i, &index));
    if (index > 10000) {
      std::cerr << "cache cleared!" << std::endl;
      index = 0;
      bf_->clearCache();
    }
  }
  bf_->clearCache();
  emit saveToFileSignal();

  // QPen pen;
  // pen.setColor(QColor(255, 0, 0, 255));
  // scene()->addEllipse(185, 50, 5, 5, pen);
  // rotate(270.0f);
  // 187 52
  // setTransform(QTransform().scale(1, 1));
}

void MainForm::renderNewImageEmitter() {
  emit renderNewImage();
}

void MainForm::saveToFile() {
  QImage image(mapFromScene(scene()->sceneRect()).boundingRect().adjusted(0, 0, -1, -1).size(), QImage::Format_ARGB32_Premultiplied);
  image.fill(0);
  QPainter painter(&image);
  render(&painter, painter.viewport(), mapFromScene(scene()->sceneRect()).boundingRect().adjusted(0, 0, -1, -1));
  image.save("image.png");
  exit(1);
}

void MainForm::populateSceneItem() {
  MainForm::image_coords img_coor;
  if (images.try_pop(img_coor)) {
    QGraphicsPixmapItem* pi = scene()->addPixmap(QPixmap::fromImage(img_coor.first.transformed(QMatrix().rotate(90 * (4 - bf_->set().rotate)))));
    pi->setFlag(QGraphicsItem::ItemIsMovable, false);
    pi->setFlag(QGraphicsItem::ItemIsSelectable, false);
    // std::cout << 16 * coor.first << " " << 16 * coor.second << std::endl;
    pi->setPos(16 * img_coor.second.first, 16 * img_coor.second.second);
    if (bf_->set().rotate == 0) {
      pi->setZValue(img_coor.second.second);
    } else if (bf_->set().rotate == 1) {
      pi->setZValue(img_coor.second.first);
    } else if (bf_->set().rotate == 2) {
      pi->setZValue(-img_coor.second.second);
    } else if (bf_->set().rotate == 3) {
      pi->setZValue(-img_coor.second.first);
    }
  } else {
    std::cerr << "must not happen!" << std::endl;
    exit(1);
  }
}

void MainForm::scale() {
  if (scale_ <= 0) {
    setTransform(QTransform().rotate(90 * bf_->set().rotate).scale(1.0/pow(2.0, abs(scale_ - 1)),
                                    1.0/pow(2.0, abs(scale_ - 1))));
  } else {
    setTransform(QTransform().rotate(90 * bf_->set().rotate).scale(scale_, scale_));
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
