#include <iostream>
#include <tbb/parallel_for.h>

uint8_t* global_image;
uint32_t g_width;
uint32_t g_height;

#include "./nbt.h"
#include "./assemble.h"

int main(int ac, char* av[]) {
  if (ac != 2) {
    std::cerr << "Usage: ./nbtparse [filename | world number]" << std::endl;
    return 1;
  }
  int world = atoi(av[1]);
  nbt bf = world ? nbt(world) : nbt(av[1]);
  std::cout << bf.string();
  bf.setSettings(getSettings());
  std::pair<int, int> min_norm, max_norm;
  calculateMinMaxPoint(min_norm, max_norm, bf);
  std::string buffer_file = "";
  uint32_t width, height;
  uint16_t header_size = writeHeader(buffer_file, min_norm, max_norm,
                                     boost::ref(width), boost::ref(height), bf);

  size_t range = static_cast<size_t>(max_norm.second - min_norm.second + 1);
  boost::progress_display show_progress(range);
  std::list<std::vector<int> > tiles(range);
  size_t tiles_nr = fillTiles(tiles, bf, min_norm, max_norm, show_progress);
  tbb::atomic<size_t> progress_index, mem_index;
  progress_index = 0;
  mem_index = 0;
  show_progress.restart(tiles_nr);
  std::list<std::vector<int> >::iterator it = tiles.begin();
  for (int i = min_norm.second; i <= max_norm.second; ++i) {
    tbb::parallel_for(tbb::blocked_range<std::vector<int>::iterator>
                                                       (it->begin(), it->end()),
                      ApplyFoo(&bf, i, &progress_index, min_norm));
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

  pamToPng(buffer_file, "test.png", header_size, width, height);
  return 0;
}
