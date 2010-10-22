#include <iostream>
#include <cstdlib>
#include <boost/thread/thread.hpp>
#include <tbb/concurrent_queue.h>

#include "./nbt.h"

typedef std::pair<Image<uint8_t>, std::pair<int, int> > image_coords;

std::pair<int, int> projectCoords(std::pair<int, int> p, int phi) {
  if (phi == 0) return p;
  int cos_phi = phi % 2 - 1;
  if (!phi) cos_phi = 1;
  int sin_phi = phi % 2;
  if (phi == 3) sin_phi = -1;
  return std::make_pair(p.first * cos_phi - p.second * sin_phi,
                        p.first * sin_phi + p.second * cos_phi);
}

int startRendering(tbb::concurrent_bounded_queue<image_coords>& images,
                   nbt& bf,
                   std::pair<int, int> min_norm,
                   std::pair<int, int> max_norm,
                   size_t header_size) {
  image_coords img_coor;
  images.pop(img_coor);
  std::pair<int, int> projected = projectCoords(
                                    std::make_pair(16 * img_coor.second.first,
                                                   16 * img_coor.second.second),
                                    bf.set().rotate);
  size_t width = img_coor.first.cols;
  size_t height = img_coor.first.rows;
  size_t nr_pixels = width * height;
  int offset_x = projected.first - min_norm.first * 16;
  int offset_y = projected.second - min_norm.second * 16;
  int g_width = (max_norm.first - min_norm.first + 1) * 16;
  int g_height = (max_norm.second - min_norm.second + 1) * 16;

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
  return 0;
}

size_t writeHeader(std::string filename,
                   std::pair<int, int> min_norm,
                   std::pair<int, int> max_norm,
                   const nbt& bf) {
  std::stringstream ss;
  int width = (max_norm.first - min_norm.first + 1) * 16;
  int height = (max_norm.second - min_norm.second + 1) * 16;
  if (bf.set().oblique) height += 128;
  ss << "P7\n"
     << "WIDTH "     << width << "\n"
     << "HEIGHT "    << height << "\n"
     << "DEPTH "     << 4 << "\n"
     << "MAXVAL "    << 255 << "\n"
     << "TUPLTYPE "  << "RGB_ALPHA" << "\n"
     << "ENDHDR"     << "\n";
  size_t header_size = ss.str().size();
  FILE* pam = fopen("test.ppm", "w");
  fwrite(ss.str().c_str(), 1, header_size, pam);
  fseek(pam, width * height * 4 - 1, SEEK_CUR);
  fwrite("", 1, 1, pam);
  fclose(pam);
  return header_size;
}

int main(int ac, char* av[]) {
  if (ac != 2) {
    std::cerr << "Usage: ./nbtparse [filename | world number]" << std::endl;
    return 1;
  }
  int world = atoi(av[1]);
  nbt bf;
  if (world == 0) {
    bf = nbt(av[1]);
  } else {
    bf = nbt(world);
  }
  std::cout << bf.string();
  Settings set;
  set.topview = false;
  set.oblique = true;
  set.heightmap = false;
  set.color = false;
  set.shadow_strength = 60;
  set.shadow_quality = true;
  set.shadow_quality_ultra = true;
  set.relief_strength = 10;
  set.sun_direction = 1;
  set.rotate = 1;
  set.sun_direction = (set.sun_direction + ((set.rotate + 3) % 4) * 2) % 8;
  if (set.shadow_strength != 0) {
    set.shadow = true;
  } else {
    set.shadow = false;
  }
  if (set.relief_strength != 0) {
    set.relief = true;
  } else {
    set.relief = false;
  }
  bf.setSettings(set);
  tbb::concurrent_bounded_queue<image_coords> images;
  std::pair<int, int> min_norm, max_norm;
  {
    std::pair<int, int> min(bf.xPos_min(), bf.zPos_min());
    std::pair<int, int> max(bf.xPos_max(), bf.zPos_max());
    min = projectCoords(min, bf.set().rotate);
    max = projectCoords(max, bf.set().rotate);
    min_norm = std::make_pair(std::min(min.first, max.first),
                              std::min(min.second, max.second));
    max_norm = std::make_pair(std::max(min.first, max.first),
                              std::max(min.second, max.second));
  }
  size_t header_size = writeHeader("test.pam", min_norm, max_norm, bf);
  boost::thread render_thread(boost::bind(&startRendering,
                                          boost::ref(images),
                                          boost::ref(bf),
                                          boost::ref(min_norm),
                                          boost::ref(max_norm),
                                          header_size));

  render_thread.join();
  return 0;
}
