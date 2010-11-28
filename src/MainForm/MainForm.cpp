/* See LICENSE file for copyright and license details. */
#include "MainForm.moc"

#include "../assemble.h"
#include <tbb/task_scheduler_init.h>

MainForm::MainForm(QGraphicsScene* img, nbt* bf, QWidget* parent_)
                 : QGraphicsView(img, parent_), bf_(bf), scale_(1),
                   images(), stop(false) {
  connect(this, SIGNAL(scaleSig()), this, SLOT(scale()));
  connect(this, SIGNAL(renderNewImage()), this, SLOT(populateSceneItem()));
  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

class ApplyFooQT {
  mutable MainForm* mainform_;
  int i_;
  mutable tbb::atomic<size_t>* index_;
 public:
  void operator()( const tbb::blocked_range<std::vector<int>::iterator>& r ) const {
    for(std::vector<int>::iterator j=r.begin(); j!=r.end(); ++j) {
      if (mainform_->stop) break;
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

void MainForm::StopPopulateScene() {
  stop = true;
}

void MainForm::populateScene() {
  stop = false;
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
  tbb::task_scheduler_init init;
  for (int i = min_norm.second; i <= max_norm.second; ++i) {
    tbb::parallel_for(tbb::blocked_range<std::vector<int>::iterator>
                                                       (it->begin(), it->end()),
                      ApplyFooQT(this, i, &progress_index));
    if (stop) break;
    ++it;
    show_progress += progress_index;
    progress_index = 0;
  }
}

void MainForm::renderNewImageEmitter() {
  emit renderNewImage();
}

void MainForm::populateSceneItem() {
  MainForm::image_coords img_coor;
  if (images.pop_if_present(img_coor)) {
    QImage img(&(img_coor.first.data[0]),
               img_coor.first.cols,
               img_coor.first.rows,
               QImage::Format_ARGB32);
    std::pair<int, int> p = projectCoords(
                                       std::make_pair(16 * img_coor.second.x(),
                                                      16 * img_coor.second.y()),
                                       bf_->set().rotate);
    if (bf_->set().isometric) {
      p = std::make_pair(2 * p.first - 2 * p.second, p.first + p.second);
    }
    QGraphicsPixmapItem* pi = new QGraphicsPixmapItem(QPixmap::fromImage(img));
    pi->setPos(p.first, p.second);
    pi->setZValue(p.second);
    pi->setFlag(QGraphicsItem::ItemIsMovable, false);
    pi->setFlag(QGraphicsItem::ItemIsSelectable, false);
    scene()->addItem(pi);
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

void MainForm::mouseMoveEvent(QMouseEvent* mevent) {
  QPoint mpos = mevent->pos();
  QPointF scene_pos_float = mapToScene(mpos);
  QPoint scene_pos(static_cast<int>(floor(scene_pos_float.x())),
                   static_cast<int>(floor(scene_pos_float.y())));
  if (bf_ && bf_->set().topview) {
    std::pair<int, int> scene_pos_mapped =
                     projectCoords(std::make_pair(scene_pos.x(), scene_pos.y()),
                                   (4 - bf_->set().rotate) % 4);
    std::cerr << scene_pos_mapped.first << " " << scene_pos_mapped.second << std::endl;
  }
}
