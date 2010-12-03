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
    std::string render_mode;
    std::string shadow_quality;
    std::string sun_direction;

    std::stringstream ss;
    std::string program_name = boost::filesystem::path(av[0]).filename();
    ss << "Usage: " << program_name << " world-number [options]" << std::endl
       << "   or: " << program_name << " filename [options]" << std::endl
                    << std::endl
                    << "Options";
    po::options_description desc(ss.str());
    desc.add_options()
      ("help,h", "produce help message\n")
      ("render-mode,m", po::value<std::string>(&render_mode)
                                            ->default_value("top-view"),
                        "'top-view'  or '0'\n"
                        "'oblique'   or '1'\n"
                        "'isometric' or '2'\n")
      ("shadow-quality,q", po::value<std::string>(&shadow_quality)
                                            ->default_value("normal"),
                        "'normal'    or '0'\n"
                        "'high'      or '1'\n"
                        "'ultra'     or '2'\n")
      ("sun-direction,d", po::value<std::string>(&sun_direction)
                                            ->default_value("NW"),
                        "'NW', 'W', 'SW', 'S',\n"
                        "'SE', 'E', 'NE', 'N'\n")
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

    Settings set;
    if (vm.count("help")) {
      std::cerr << visible << std::endl;
      return 1;
    }
    if (!vm.count("world-number")) {
      std::cerr << visible << std::endl;
      return 1;
    }
    if (!render_mode.compare("top-view") || !render_mode.compare("0")) {
      set.topview = true;
      set.oblique = false;
      set.isometric = false;
    } else if (!render_mode.compare("oblique") || !render_mode.compare("1")) {
      set.topview = false;
      set.oblique = true;
      set.isometric = false;
    } else if (!render_mode.compare("isometric") || !render_mode.compare("2")) {
      set.topview = false;
      set.oblique = false;
      set.isometric = true;
    } else {
      std::cerr << visible << std::endl;
      return 1;
    }
    if (!shadow_quality.compare("normal") || !shadow_quality.compare("0")) {
      set.shadow_quality = false;
      set.shadow_quality_ultra = false;
    } else if (!shadow_quality.compare("high") || !shadow_quality.compare("1")) {
      set.shadow_quality = true;
      set.shadow_quality_ultra = false;
    } else if (!shadow_quality.compare("ultra") || !shadow_quality.compare("2")) {
      set.shadow_quality = true;
      set.shadow_quality_ultra = true;
    } else {
      std::cerr << visible << std::endl;
      return 1;
    }
    if (!sun_direction.compare("NW")) {
      set.sun_direction = 1;
    } else if (!sun_direction.compare("W")) {
      set.sun_direction = 2;
    } else if (!sun_direction.compare("SW")) {
      set.sun_direction = 3;
    } else if (!sun_direction.compare("S")) {
      set.sun_direction = 4;
    } else if (!sun_direction.compare("SE")) {
      set.sun_direction = 5;
    } else if (!sun_direction.compare("E")) {
      set.sun_direction = 6;
    } else if (!sun_direction.compare("NE")) {
      set.sun_direction = 7;
    } else if (!sun_direction.compare("N")) {
      set.sun_direction = 0;
    } else {
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
    bf.setSettings(set);
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
