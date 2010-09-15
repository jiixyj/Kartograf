#include "MainForm.moc"

#include <iostream>
#include <QtGui>


MainForm::MainForm(QGraphicsScene* img, nbt* bf, QWidget* parent)
                 : QGraphicsView(img, parent), bf_(bf), scale_(1) {
  connect(this, SIGNAL(scaleSig()), this, SLOT(scale()));
}

void MainForm::populateScene() {
  for (int i = bf_->zPos_min(); i <= bf_->zPos_max(); ++i) {
    for (int j = bf_->xPos_min(); j <= bf_->xPos_max(); ++j) {
      populateSceneItem(i, j);
      QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
  }
  bf_->clearCache();
  // QPen pen;
  // pen.setColor(QColor(255, 0, 0, 255));
  // scene()->addEllipse(185, 50, 5, 5, pen);
  // rotate(270.0f);
  // 187 52
  // setTransform(QTransform().scale(1, 1));
}

void MainForm::populateSceneItem(int i, int j) {
      QGraphicsPixmapItem* pi =
      scene()->addPixmap(QPixmap::fromImage(bf_->getImage(j, i)));
      pi->setFlag(QGraphicsItem::ItemIsMovable, false);
      pi->setFlag(QGraphicsItem::ItemIsSelectable, false);
      pi->setPos(16 * j, 16 * i);
}

void MainForm::scale() {
  if (scale_ <= 0) {
    setTransform(QTransform().scale(1.0/pow(2.0, abs(scale_ - 1)), 1.0/pow(2.0, abs(scale_ - 1))));
  } else {
    setTransform(QTransform().scale(scale_, scale_));
  }
}

void MainForm::getGoing() {
  emit startPopulatingScene();
}

void MainForm::mousePressEvent(QMouseEvent* event) {
  switch (event->button()) {
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

void MainForm::mouseDoubleClickEvent(QMouseEvent* event) {
  mousePressEvent(event);
  return;
}
