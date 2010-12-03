/* See LICENSE file for copyright and license details. */
#include "./assemble.h"

#include <cstdio>
#include <tbb/tbb_exception.h>
#include <tbb/task_scheduler_init.h>
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>

namespace po = boost::program_options;

int main(int ac, char* av[]) {
  try {
    std::string world;
    std::stringstream ss;
    std::string program_name = boost::filesystem::path(av[0]).filename();
    ss << "Usage: " << program_name << " world-number [options]" << std::endl
       << "   or: " << program_name << " filename [options]" << std::endl
                    << std::endl
                    << "Options";
    po::options_description desc(ss.str());
    desc.add_options()
      ("help,h", "produce help message")
    ;
    po::options_description hidden("Hidden options");
    hidden.add_options()
      ("world-number,w", po::value<std::string>(&world),
                         "Minecraft world number")
    ;

    po::positional_options_description p;
    p.add("world-number", 1);
    po::options_description all;
    all.add(desc).add(hidden);
    po::options_description visible;
    visible.add(desc);

    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).options(all).positional(p)
                                             .allow_unregistered().run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
      std::cerr << visible << std::endl;
      return 1;
    }

    if (!vm.count("world-number")) {
      std::cerr << visible << std::endl;
      return 1;
    }
    int world_number;
    try {
      world_number = boost::lexical_cast<int>(world);
      if (world_number > 5 || world_number < 1) {
        std::cerr << "World number must lie between 1 and 5!" << std::endl;
        return 1;
      }
    } catch (boost::bad_lexical_cast& e) {
      world_number = 0;
    }

    nbt bf = world_number ? nbt(world_number) : nbt(av[1]);
    if (bf.bad_world) {
      std::cerr << "Invalid World!" << std::endl;
      return 1;
    }
    std::cout << bf.string();
    bf.setSettings(getSettings());
    std::pair<int, int> min_norm, max_norm;
    calculateMinMaxPoint(min_norm, max_norm, bf);
    std::string buffer_file = "tmp.pam";

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
