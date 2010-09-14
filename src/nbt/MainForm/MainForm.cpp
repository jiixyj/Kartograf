#include "MainForm.moc"

#include <iostream>
#include <Qt/QtTest>


MainForm::MainForm(QGraphicsScene* img, nbt* bf, QWidget* parent)
                 : QGraphicsView(img, parent), bf_(bf) {
}

void MainForm::populateScene() {
  for (int i = bf_->zPos_min(); i <= bf_->zPos_max(); ++i) {
    for (int j = bf_->xPos_min(); j <= bf_->xPos_max(); ++j) {
      populateSceneH(i, j);
    }
   // QTest::qWait(1);
  }
}

void MainForm::populateSceneH(int i, int j) {
      QGraphicsPixmapItem* pi =
      scene()->addPixmap(QPixmap::fromImage(bf_->getImage(j, i)));
      pi->setFlag(QGraphicsItem::ItemIsMovable, false);
      pi->setFlag(QGraphicsItem::ItemIsSelectable, false);
      pi->setPos(16 * j, 16 * i);
}

void MainForm::getGoing() {
  emit startPopulatingScene();
}
