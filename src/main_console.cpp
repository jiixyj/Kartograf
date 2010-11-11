#include "./assemble.h"

#include <cstdio>
#include <tbb/tbb_exception.h>
#include <tbb/task_scheduler_init.h>

int main(int ac, char* av[]) {
  try {
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

    size_t range = static_cast<size_t>(max_norm.second - min_norm.second + 1);
    boost::progress_display show_progress(range);
    std::list<std::vector<int> > tiles(range);
    size_t tiles_nr = fillTiles(tiles, bf, min_norm, max_norm, show_progress);

    writeHeader(buffer_file, min_norm, max_norm, bf);
    FILE* out_file = fopen("test.png", "wb");
    if (!out_file) {
      std::cerr << "Could not open output image file!" << std::endl;
      return 1;
    }

    tbb::atomic<size_t> progress_index, mem_index;
    progress_index = 0;
    mem_index = 0;
    show_progress.restart(tiles_nr);
    std::list<std::vector<int> >::iterator it = tiles.begin();
    tbb::task_scheduler_init init;
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

    pamToPng(out_file);
  } catch (std::runtime_error e) {
    std::cerr << e.what() << std::endl;
    return 1;
  } catch (tbb::captured_exception e) {
    std::cerr << "In tbb loop: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
