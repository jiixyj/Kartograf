#include <png.h>
#include <iostream>
#include <cstdlib>
#include <boost/thread/thread.hpp>
#include <boost/progress.hpp>
#include <tbb/concurrent_queue.h>
#include <tbb/parallel_for.h>

#include "./nbt.h"


std::pair<int, int> projectCoords(std::pair<int, int> p, int phi) {
  if (phi == 0) return p;
  int cos_phi = phi % 2 - 1;
  if (!phi) cos_phi = 1;
  int sin_phi = phi % 2;
  if (phi == 3) sin_phi = -1;
  return std::make_pair(p.first * cos_phi - p.second * sin_phi,
                        p.first * sin_phi + p.second * cos_phi);
}

typedef std::pair<Image<uint8_t>, std::pair<int, int> > image_coords;

uint8_t* global_image;

int startRendering(std::string filename,
                   tbb::concurrent_bounded_queue<image_coords>& images,
                   nbt& bf,
                   std::pair<int, int> min_norm,
                   std::pair<int, int> max_norm,
                   uint16_t header_size) {
  image_coords img_coor;
  int g_width = (max_norm.first - min_norm.first + 1) * 16;
  int g_height = (max_norm.second - min_norm.second + 1) * 16;
  if (!filename.size()) {
    global_image = reinterpret_cast<uint8_t*>
                   (malloc(static_cast<size_t>(g_width * g_height * 4)));
  }
  for (;;) {
    images.pop(img_coor);
    if (img_coor.first.channels == 0) {
      break;
    }
    std::pair<int, int> projected = projectCoords(
                                    std::make_pair(16 * img_coor.second.first,
                                                   16 * img_coor.second.second),
                                    bf.set().rotate);
    uint16_t width = img_coor.first.cols;
    uint16_t height = img_coor.first.rows;
    int offset_x = projected.first - min_norm.first * 16;
    int offset_y = projected.second - min_norm.second * 16;

    for (size_t i = 0; i < 1u * width * height; ++i) {
      size_t index = i * 4;
      std::swap(img_coor.first.data[index], img_coor.first.data[index + 2]);
    }
    if (filename.size()) {
      FILE* pam = fopen(filename.c_str(), "r+b");
      fseek(pam, header_size, SEEK_CUR);
      fseek(pam, (offset_y * g_width + offset_x) * 4, SEEK_CUR);
      for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
          if (img_coor.first.data[i * width * 4 + j * 4 + 3] != 0) {
            fwrite(&(img_coor.first.data[i * width * 4 + j * 4]), 4, 1, pam);
          } else {
            fseek(pam, 4, SEEK_CUR);
          }
        }
        fseek(pam, (g_width - width) * 4, SEEK_CUR);
      }
      fclose(pam);
    } else {
      long pos = (offset_y * g_width + offset_x) * 4;
      for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
          if (img_coor.first.data[i * width * 4 + j * 4 + 3] != 0) {
            memcpy(global_image + pos,
                   &img_coor.first.data[i * width * 4 + j * 4],
                   4);
          }
          pos += 4;
        }
        pos += (g_width - width) * 4;
      }
    }
  }
  return 0;
}

uint16_t writeHeader(std::string filename,
                   std::pair<int, int> min_norm,
                   std::pair<int, int> max_norm,
                   uint32_t& width, uint32_t& height,
                   const nbt& bf) {
  width =  static_cast<uint32_t>(max_norm.first - min_norm.first + 1) * 16;
  height = static_cast<uint32_t>(max_norm.second - min_norm.second + 1) * 16;
  if (bf.set().oblique) height += 128;
  uint16_t header_size = 0;
  if (filename.size()) {
    std::stringstream ss;
    ss << "P7\n"
       << "WIDTH "     << width << "\n"
       << "HEIGHT "    << height << "\n"
       << "DEPTH "     << 4 << "\n"
       << "MAXVAL "    << 255 << "\n"
       << "TUPLTYPE "  << "RGB_ALPHA" << "\n"
       << "ENDHDR"     << "\n";
    header_size = static_cast<uint16_t>(ss.str().size());
    FILE* pam = fopen(filename.c_str(), "wb");
    fwrite(ss.str().c_str(), 1, header_size, pam);
    fseek(pam, static_cast<long>(width * height * 4 - 1), SEEK_CUR);
    fwrite("", 1, 1, pam);
    fclose(pam);
  }
  return header_size;
}

class ApplyFoo {
  nbt* bf_;
  tbb::concurrent_bounded_queue<image_coords>* images_;
  int i_;
  tbb::atomic<size_t>* index_;
 public:
  void operator()( const tbb::blocked_range<std::vector<int>::iterator>& r ) const {
    for(std::vector<int>::iterator j=r.begin(); j!=r.end(); ++j) {
      bool result = false;
      std::pair<int, int> bp = projectCoords(std::make_pair(*j, i_),
                                        (4 - bf_->set().rotate) % 4);
      const Image<uint8_t>& image = bf_->getImage(bp.first, bp.second, &result);
      if (!result) {
        continue;
      }
      *index_ += 1;
      images_->push(image_coords(image, std::make_pair(bp.first, bp.second)));
    }
  }
  ApplyFoo(nbt* bf, tbb::concurrent_bounded_queue<image_coords>* images, int i,
           tbb::atomic<size_t>* index)
          : bf_(bf), images_(images), i_(i), index_(index) {}
 private:
  ApplyFoo& operator=(const ApplyFoo&);
};

Settings getSettings() {
  Settings set;
  set.topview = true;
  set.oblique = false;
  set.heightmap = false;
  set.color = false;
  set.shadow_strength = 0;
  set.shadow_quality = true;
  set.shadow_quality_ultra = false;
  set.relief_strength = 10;
  set.sun_direction = 1;
  set.rotate = 1;
  set.sun_direction = (set.sun_direction + ((set.rotate + 3) % 4) * 2) % 8;
  set.shadow = set.shadow_strength;
  set.relief = set.relief_strength;
  return set;
}

void calculateMinMaxPoint(std::pair<int, int>& min_norm,
                          std::pair<int, int>& max_norm,
                          const nbt& bf) {
  std::pair<int, int> min(bf.xPos_min(), bf.zPos_min());
  std::pair<int, int> max(bf.xPos_max(), bf.zPos_max());
  min = projectCoords(min, bf.set().rotate);
  max = projectCoords(max, bf.set().rotate);
  min_norm = std::make_pair(std::min(min.first, max.first),
                            std::min(min.second, max.second));
  max_norm = std::make_pair(std::max(min.first, max.first),
                            std::max(min.second, max.second));
}

void pamToPng(std::string pam_name, std::string png_name, uint16_t header_size,
              uint32_t width, uint32_t height) {
  FILE* pam = NULL;
  if (pam_name.size()) {
    pam = fopen(pam_name.c_str(), "rb");
    fseek(pam, header_size, SEEK_CUR);
  }
  FILE* out = fopen(png_name.c_str(), "wb");
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
  for (size_t i = 0; i < height; ++i) {
    if (pam) {
      fread(pngRow, 4, width, pam);
    } else {
      memcpy(pngRow, global_image + i * width * 4, width * 4);
    }
    png_write_row(pngP, pngRow);
  }
  free(pngRow);

  png_write_end(pngP, infoP);
  png_destroy_write_struct(&pngP, &infoP);
  fclose(out);
  if (pam) {
    fclose(pam);
  } else {
    free(global_image);
  }
}

size_t fillTiles(std::list<std::vector<int> >& tiles, const nbt& bf,
                 const std::pair<int, int>& min_norm,
                 const std::pair<int, int>& max_norm,
                 boost::progress_display& show_progress) {
  size_t tiles_nr = 0;
  std::list<std::vector<int> >::iterator it = tiles.begin();
  for (int i = min_norm.second; i <= max_norm.second; ++i) {
    for (int j = min_norm.first; j <= max_norm.first; ++j) {
      std::pair<int, int> p = projectCoords(std::make_pair(j, i),
                                            (4 - bf.set().rotate) % 4);
      boost::filesystem::path path;
      if (bf.exists(p.first, p.second, path)) {
        p = projectCoords(p, bf.set().rotate);
        it->push_back(p.first);
      }
    }
    tiles_nr += it->size();
    ++it;
    ++show_progress;
  }
  return tiles_nr;
}

int main(int ac, char* av[]) {
  if (ac != 2) {
    std::cerr << "Usage: ./nbtparse [filename | world number]" << std::endl;
    return 1;
  }
  int world = atoi(av[1]);
  nbt bf = world ? nbt(world) : nbt(av[1]);
  std::cout << bf.string();
  bf.setSettings(getSettings());
  tbb::concurrent_bounded_queue<image_coords> images;
  std::pair<int, int> min_norm, max_norm;
  calculateMinMaxPoint(min_norm, max_norm, bf);
  std::string buffer_file = "";
  uint32_t width, height;
  uint16_t header_size = writeHeader(buffer_file, min_norm, max_norm,
                                     width, height, bf);

  size_t range = static_cast<size_t>(max_norm.second - min_norm.second + 1);
  boost::progress_display show_progress(range);
  std::list<std::vector<int> > tiles(range);
  size_t tiles_nr = fillTiles(tiles, bf, min_norm, max_norm, show_progress);
  boost::thread render_thread(boost::bind(&startRendering, buffer_file,
                                          boost::ref(images), boost::ref(bf),
                                          boost::ref(min_norm),
                                          boost::ref(max_norm), header_size));
  tbb::atomic<size_t> progress_index, mem_index;
  progress_index = 0;
  mem_index = 0;
  show_progress.restart(tiles_nr);
  std::list<std::vector<int> >::iterator it = tiles.begin();
  for (int i = min_norm.second; i <= max_norm.second; ++i) {
    tbb::parallel_for(tbb::blocked_range<std::vector<int>::iterator>
                                                       (it->begin(), it->end()),
                      ApplyFoo(&bf, &images, i, &progress_index));
    mem_index += progress_index;
    if (mem_index > 10000) {
      mem_index = 0;
      bf.clearCache();
    }
    ++it;
    show_progress += progress_index;
    progress_index = 0;
  }
  bf.clearCache();
  images.push(image_coords(Image<uint8_t>(0, 0, 0), std::make_pair(0, 0)));
  render_thread.join();

  pamToPng(buffer_file, "test.png", header_size, width, height);
  return 0;
}
