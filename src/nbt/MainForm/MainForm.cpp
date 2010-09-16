#include "MainForm.moc"

#include <QtGui>
#include <iostream>

MainForm::MainForm(QGraphicsScene* img, nbt* bf, QWidget* parent_)
                 : QGraphicsView(img, parent_), scene_(), bf_(bf), scale_(1) {
  connect(this, SIGNAL(scaleSig()), this, SLOT(scale()));
  connect(this, SIGNAL(renderNewImage()), this, SLOT(populateSceneItem()));
  connect(this, SIGNAL(saveToFileSignal()), this, SLOT(saveToFile()));
}

void MainForm::populateScene() {
  int index = 0;
  for (int i = bf_->zPos_min(); i <= bf_->zPos_max(); ++i) {
    int j;
    #pragma omp parallel for
    for (j = bf_->xPos_min(); j <= bf_->xPos_max(); ++j) {
      bool result = false;
      const QImage& img = bf_->getImage(j, i, &result);
      if (!result) continue;
      #pragma omp atomic
      index += 1;
      #pragma omp critical
      {
        images.push(img);
        coords.push(QPair<int, int>(j, i));
      }
      #pragma omp critical
      emit renderNewImage();
    }
    if (index > 10000) {
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

void MainForm::saveToFile() {
  QImage image(scene()->sceneRect().toAlignedRect().size(), QImage::Format_ARGB32_Premultiplied);
  image.fill(0);
  QPainter painter(&image);
  scene()->render(&painter);
  image.save("image.png");
  exit(1);
}

void MainForm::populateSceneItem() {
  QImage img;
  QPair<int, int> coor;
  if (images.try_pop(img) && coords.try_pop(coor)) {
    QGraphicsPixmapItem* pi = scene()->addPixmap(QPixmap::fromImage(img));
    pi->setFlag(QGraphicsItem::ItemIsMovable, false);
    pi->setFlag(QGraphicsItem::ItemIsSelectable, false);
    // std::cout << 16 * coor.first << " " << 16 * coor.second << std::endl;
    pi->setPos(16 * coor.first, 16 * coor.second);
  } else {
    std::cerr << "must not happen!" << std::endl;
    exit(1);
  }
}

void MainForm::scale() {
  if (scale_ <= 0) {
    setTransform(QTransform().scale(1.0/pow(2.0, abs(scale_ - 1)),
                                    1.0/pow(2.0, abs(scale_ - 1))));
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
