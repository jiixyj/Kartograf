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

    std::stringstream ss;
    int width = (max_norm.x() - min_norm.x() + 1) * 16;
    int height = (max_norm.y() - min_norm.y() + 1) * 16;
    if (bf_->set().oblique) height += 128;
    ss << "P7\n"
       << "WIDTH "     << width << "\n"
       << "HEIGHT "    << height << "\n"
       << "DEPTH "     << 4 << "\n"
       << "MAXVAL "    << 255 << "\n"
       << "TUPLTYPE "  << "RGB_ALPHA" << "\n"
       << "ENDHDR"     << "\n";
    header_size = ss.str().size();
    FILE* pam = fopen("test.ppm", "w");
    fwrite(ss.str().c_str(), 1, header_size, pam);
    fseek(pam, width * height * 4 - 1, SEEK_CUR);
    fwrite("", 1, 1, pam);
    fclose(pam);

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

  // QPen pen;
  // pen.setColor(QColor(255, 0, 0, 255));
  // scene()->addEllipse(185, 50, 5, 5, pen);
  // rotate(270.0f);
  // 187 52
  // setTransform(QTransform().scale(1, 1));

  size_t nr_pixels = width * height;
  pam = fopen("test.ppm", "r");
  FILE* out = fopen("test.png", "w");
  fseek(pam, header_size, SEEK_CUR);
  png_struct* pngP;
  png_info* infoP;
  pngP = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  infoP = png_create_info_struct(pngP);
  png_set_IHDR(pngP, infoP, width, height, 8,
               PNG_COLOR_TYPE_RGB_ALPHA,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);
  png_init_io(pngP, out);
  png_write_info(pngP, infoP);

  png_byte* pngRow = reinterpret_cast<png_byte*>(malloc(width * 4));
  for (int i = 0; i < height; ++i) {
    fread(pngRow, 4, width, pam);
    png_write_row(pngP, pngRow);
  }
  free(pngRow);

  png_write_end(pngP, infoP);
  png_destroy_write_struct(&pngP, &infoP);
  fclose(pam);
  fclose(out);
}

void MainForm::renderNewImageEmitter() {
  emit renderNewImage();
}

void MainForm::saveToFile() {
  scale_ = 1;
  scale();
  QImage image(mapFromScene(scene()->sceneRect()).boundingRect().adjusted(0, 0, -1, -1).size(), QImage::Format_ARGB32);
  image.fill(0);
  QPainter painter(&image);
  render(&painter, painter.viewport(), mapFromScene(scene()->sceneRect()).boundingRect().adjusted(0, 0, -1, -1));
  // image.save("image.png");
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

    size_t width = img_coor.first.cols;
    size_t height = img_coor.first.rows;
    size_t nr_pixels = width * height;
    int offset_x = projected.x() - min_norm.x() * 16;
    int offset_y = projected.y() - min_norm.y() * 16;
    int g_width = (max_norm.x() - min_norm.x() + 1) * 16;
    int g_height = (max_norm.y() - min_norm.y() + 1) * 16;

    FILE* pam = fopen("test.ppm", "r+");
    fseek(pam, header_size, SEEK_CUR);
    for (size_t i = 0; i < nr_pixels; ++i) {
      size_t index = i * 4;
      std::swap(img_coor.first.data[index], img_coor.first.data[index + 2]);
    }
    fseek(pam, offset_y * g_width * 4 + offset_x * 4, SEEK_CUR);
    for (size_t i = 0; i < height; ++i) {
      for (size_t j = 0; j < width; ++j) {
        if (img_coor.first.data[i * width * 4 + j * 4 + 3] != 0) {
          fwrite(&(img_coor.first.data[i * width * 4 + j * 4]), 4, 1, pam);
        } else {
          fseek(pam, 4, SEEK_CUR);
        }
      }
      fseek(pam, g_width * 4 - width * 4, SEEK_CUR);
    }
    fclose(pam);
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
