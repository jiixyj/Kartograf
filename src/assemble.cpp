#include "./assemble.h"

#include <png.h>

#include <fstream>
#include <tbb/mutex.h>

uint8_t* global_image;
int32_t* global_image_depth;
int32_t g_width;
int32_t g_height;
int32_t g_x_min = 0;
int32_t g_y_min = 0;
int32_t g_x_max = 0;
int32_t g_y_max = 0;
uint16_t g_header_size = 0;
std::string g_filename = "";

std::pair<int, int> projectCoords(std::pair<int, int> p, int phi) {
  if (phi == 0) return p;
  int cos_phi = phi % 2 - 1;
  if (!phi) cos_phi = 1;
  int sin_phi = phi % 2;
  if (phi == 3) sin_phi = -1;
  return std::make_pair(p.first * cos_phi - p.second * sin_phi,
                        p.first * sin_phi + p.second * cos_phi);
}

int render_tile(Image<uint8_t>& image,
                std::pair<int, int> projected) {
  uint16_t width = image.cols;
  uint16_t height = image.rows;

  static tbb::mutex render_mutex;

  for (size_t i = 0; i < 1u * width * height; ++i) {
    size_t index = i * 4;
    std::swap(image.data[index], image.data[index + 2]);
  }
  int64_t pos = (projected.second * g_width
               + projected.first) * 4l;
  tbb::mutex::scoped_lock lock(render_mutex);
  if (g_filename.size()) {
    std::fstream pam(g_filename.c_str());
    pam.seekp(g_header_size + pos, std::ios_base::cur);
    for (size_t i = 0; i < height; ++i) {
      for (size_t j = 0; j < width; ++j) {
        if (image.data[i * width * 4 + j * 4 + 3] != 0 &&
            global_image_depth[pos >> 2] <= projected.second) {
          pam.write(reinterpret_cast<const char*>
                    (&(image.data[i * width * 4 + j * 4])), 4);
        } else {
          pam.seekp(4, std::ios_base::cur);
        }
      }
      pam.seekp((g_width - width) * 4, std::ios_base::cur);
    }
  } else {
    for (size_t i = 0; i < height; ++i) {
      for (size_t j = 0; j < width; ++j) {
        if (image.data[i * width * 4 + j * 4 + 3] != 0 &&
            global_image_depth[pos >> 2] <= projected.second) {
          memcpy(global_image + pos, &image.data[i * width * 4 + j * 4], 4);
          global_image_depth[pos >> 2] = projected.second;
        }
        pos += 4;
      }
      pos += (g_width - width) * 4;
    }
  }
  return 0;
}

uint16_t writeHeader(std::string filename,
                   std::pair<int, int> min_norm,
                   std::pair<int, int> max_norm,
                   const nbt& bf) {
  if (bf.set().isometric) {
    g_width =  g_x_max - g_x_min + 64;
    g_height = g_y_max - g_y_min + 288;
  } else {
    g_width =  (max_norm.first - min_norm.first + 1) * 16;
    g_height = (max_norm.second - min_norm.second + 1) * 16;
    if (bf.set().oblique) g_height += 128;
  }
  uint16_t header_size = 0;
  if (filename.size()) {
    g_filename = filename;
    std::stringstream ss;
    ss << "P7\n"
       << "WIDTH "     << g_width << "\n"
       << "HEIGHT "    << g_height << "\n"
       << "DEPTH "     << 4 << "\n"
       << "MAXVAL "    << 255 << "\n"
       << "TUPLTYPE "  << "RGB_ALPHA" << "\n"
       << "ENDHDR"     << "\n";
    g_header_size = header_size = static_cast<uint16_t>(ss.str().size());
    std::ofstream pam(filename.c_str());
    pam << ss.str();

    pam.seekp(g_width * g_height * 4 - 1, std::ios_base::cur);
    pam.put('\0');
    global_image_depth = reinterpret_cast<int32_t*>
                         (calloc(static_cast<size_t>(g_width * g_height), sizeof(int32_t)));
  } else {
    global_image = reinterpret_cast<uint8_t*>
                   (calloc(static_cast<size_t>(g_width * g_height), 4));
    global_image_depth = reinterpret_cast<int32_t*>
                         (calloc(static_cast<size_t>(g_width * g_height), sizeof(int32_t)));
    if (!global_image || !global_image_depth) {
      throw std::runtime_error("Memory allocation error");
    }
  }
  return header_size;
}

void ApplyFoo::operator() (const tbb::blocked_range<std::vector<int>
                                                         ::iterator>& r) const {
  for (std::vector<int>::iterator j = r.begin(); j != r.end(); ++j) {
    bool result = false;
    std::pair<int, int> bp = projectCoords(std::make_pair(*j, i_),
                                      (4 - bf_->set().rotate) % 4);
    Image<uint8_t> image = bf_->getImage(bp.first, bp.second, &result);
    if (!result) {
      continue;
    }
    *index_ += 1;
    bp = projectCoords(bp, bf_->set().rotate);
    int offset_x = (bp.first - min_norm_.first) * 16;
    int offset_y = (bp.second - min_norm_.second) * 16;
    if (bf_->set().isometric) {
      bp = std::make_pair(2 * bp.first - 2 * bp.second, bp.first + bp.second);
      offset_x = bp.first * 16 - g_x_min;
      offset_y = bp.second * 16 - g_y_min;
    }
    render_tile(image, std::make_pair(offset_x, offset_y));
  }
}
ApplyFoo::ApplyFoo(nbt* bf, int i, tbb::atomic<size_t>* index,
                   std::pair<int, int> min_norm)
        : bf_(bf), i_(i), index_(index), min_norm_(min_norm) {}

Settings getSettings() {
  Settings set;
  set.topview = false;
  set.oblique = false;
  set.isometric = true;
  set.heightmap = false;
  set.color = false;
  set.shadow_strength = 60;
  set.shadow_quality = true;
  set.shadow_quality_ultra = true;
  set.relief_strength = 10;
  set.sun_direction = 2;
  set.rotate = 1;
  set.nightmode = 0;
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

void pamToPng(std::string png_name) {
  FILE* pam = NULL;
  if (g_filename.size()) {
    pam = fopen(g_filename.c_str(), "rb");
    fseek(pam, g_header_size, SEEK_CUR);
  }
  FILE* out = fopen(png_name.c_str(), "wb");
  png_struct* pngP;
  png_info* infoP;
  fprintf(stderr, "Set up PNG structs\n");
  pngP = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  infoP = png_create_info_struct(pngP);
  fprintf(stderr, "Set up PNG IHDR\n");
  png_set_IHDR(pngP, infoP, static_cast<png_uint_32>(g_width),
                            static_cast<png_uint_32>(g_height), 8,
               PNG_COLOR_TYPE_RGB_ALPHA,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);
  fprintf(stderr, "Init IO\n");
  png_init_io(pngP, out);
  fprintf(stderr, "Write Info\n");
  png_write_info(pngP, infoP);

  fprintf(stderr, "Write rows\n");
  png_byte* pngRow = new png_byte[g_width * 4];
  for (int32_t i = 0; i < g_height; ++i) {
    if (pam) {
      fread(pngRow, 4, static_cast<size_t>(g_width), pam);
    } else {
      memcpy(pngRow, &global_image[static_cast<size_t>(i)
                                 * static_cast<size_t>(g_width) * 4],
             static_cast<size_t>(g_width) * 4);
    }
    png_write_row(pngP, pngRow);
  }
  delete[] pngRow;

  fprintf(stderr, "Write end\n");
  png_write_end(pngP, infoP);
  png_destroy_write_struct(&pngP, &infoP);
  fclose(out);
  if (pam) {
    fclose(pam);
    free(global_image_depth);
  } else {
    free(global_image);
    free(global_image_depth);
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
        if (bf.set().isometric) {
          p = std::make_pair(32 * p.first - 32 * p.second,
                             16 * (p.first + p.second));
        }
        if (p.first < g_x_min) {
          g_x_min = p.first;
        }
        if (p.second < g_y_min) {
          g_y_min = p.second;
        }
        if (p.first > g_x_max) {
          g_x_max = p.first;
        }
        if (p.second > g_y_max) {
          g_y_max = p.second;
        }
      }
    }
    tiles_nr += it->size();
    ++it;
    ++show_progress;
  }
  return tiles_nr;
}

