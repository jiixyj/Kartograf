#include "MainForm.moc"

#include "../assemble.h"

MainForm::MainForm(QGraphicsScene* img, nbt* bf, QWidget* parent_)
                 : QGraphicsView(img, parent_), scene_(), bf_(bf), scale_(1),
                   images() {
  connect(this, SIGNAL(scaleSig()), this, SLOT(scale()));
  connect(this, SIGNAL(renderNewImage()), this, SLOT(populateSceneItem()));
  connect(this, SIGNAL(saveToFileSignal()), this, SLOT(saveToFile()));
  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

class ApplyFooQT {
  mutable MainForm* mainform_;
  int i_;
  mutable tbb::atomic<size_t>* index_;
 public:
  void operator()( const tbb::blocked_range<std::vector<int>::iterator>& r ) const {
    for(std::vector<int>::iterator j=r.begin(); j!=r.end(); ++j) {
      bool result = false;
      std::pair<int, int> bp = projectCoords(std::make_pair(*j, i_),
                                        (4 - mainform_->bf_->set().rotate) % 4);
      Image<uint8_t> image = mainform_->bf_->getImage(bp.first, bp.second, &result);
      if (!result) {
        continue;
      }
      *index_ += 1;
      mainform_->images.push(MainForm::image_coords(image, QPoint(bp.first,
                                                                  bp.second)));
      mainform_->renderNewImageEmitter();
    }
  }
  ApplyFooQT(MainForm* mainform, int i, tbb::atomic<size_t>* index)
          : mainform_(mainform), i_(i), index_(index) {}
  /* just for the compiler */
  ApplyFooQT(const ApplyFooQT& rhs)
          : mainform_(rhs.mainform_), i_(rhs.i_), index_(rhs.index_) {}
 private:
  ApplyFooQT& operator=(const ApplyFoo&);
};


void MainForm::populateScene() {
  std::pair<int, int> min_norm, max_norm;
  calculateMinMaxPoint(min_norm, max_norm, *bf_);
  size_t range = static_cast<size_t>(max_norm.second - min_norm.second + 1);
  boost::progress_display show_progress(range);
  std::list<std::vector<int> > tiles(range);
  size_t tiles_nr = fillTiles(tiles, *bf_, min_norm, max_norm, show_progress);
  tbb::atomic<size_t> progress_index, mem_index;
  progress_index = 0;
  mem_index = 0;
  show_progress.restart(tiles_nr);
  std::list<std::vector<int> >::iterator it = tiles.begin();
  for (int i = min_norm.second; i <= max_norm.second; ++i) {
    tbb::parallel_for(tbb::blocked_range<std::vector<int>::iterator>
                                                       (it->begin(), it->end()),
                      ApplyFooQT(this, i, &progress_index));
    mem_index += progress_index;
    if (mem_index > 10000) {
      mem_index = 0;
      bf_->clearCache();
    }
    ++it;
    show_progress += progress_index;
    progress_index = 0;
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
  // throw std::runtime_error("image saved!");
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
    std::pair<int, int> p = projectCoords(
                                       std::make_pair(16 * img_coor.second.x(),
                                                      16 * img_coor.second.y()),
                                       bf_->set().rotate);
    p = std::make_pair(2 * p.first - 2 * p.second, p.first + p.second);
    pi->setPos(p.first, p.second);
    pi->setZValue(p.second);
    // if (bf_->set().rotate == 0) {
    //   pi->setZValue(img_coor.second.y());
    // } else if (bf_->set().rotate == 1) {
    //   pi->setZValue(img_coor.second.x());
    // } else if (bf_->set().rotate == 2) {
    //   pi->setZValue(-img_coor.second.y());
    // } else if (bf_->set().rotate == 3) {
    //   pi->setZValue(-img_coor.second.x());
    // }
  } else {
    throw std::runtime_error("must not happen in populateSceneItem()!");
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
