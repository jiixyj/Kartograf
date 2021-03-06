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
    int shadow_strength, relief_strength;
    std::string rotation;

    std::stringstream ss;
    std::string program_name = boost::filesystem::path(av[0]).filename();
    ss << "Usage: " << program_name << " world-number [options]" << std::endl
       << "   or: " << program_name << " directory-name [options]" << std::endl
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
                        "where the sun light comes from (note: this direction "
                        "is already relative to the rotated map):\n"
                        "'NW', 'W', 'SW', 'S',\n"
                        "'SE', 'E', 'NE', 'N'\n")
      ("shadow-strength,s", po::value<int>(&shadow_strength)
                                            ->default_value(60),
                        "set to 0 to disable shadows\n"
                        "set higher to make shadows stronger\n")
      ("relief-strength,l", po::value<int>(&relief_strength)
                                            ->default_value(10),
                        "set to 0 to disable relief effect\n"
                        "set higher to make relief effect stronger\n")
      ("rotation,r", po::value<std::string>(&rotation)
                                            ->default_value("N"),
                        "the top of the map points to:\n"
                        "'N', 'W', 'S', 'E'\n")
      ("night-mode,n",  "enables night mode")
      ("height-map-mono",  "renders monochrome height map")
      ("height-map-color",  "renders color height map")
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
    try {
      po::store(po::command_line_parser(ac, av).options(all).positional(p)
                                               .run(), vm);
      po::notify(vm);
    } catch(po::error e) {
      std::cerr << e.what() << std::endl;
      std::cerr << visible << std::endl;
      return 1;
    }

    Settings set;
    if (vm.count("help")) {
      std::cerr << visible << std::endl;
      return 1;
    }
    if (!vm.count("world-number")) {
      std::cerr << visible << std::endl;
      return 1;
    }
    if (!render_mode.compare("0")) {
      render_mode = "top-view";
    } else if (!render_mode.compare("1")) {
      render_mode = "oblique";
    } else if (!render_mode.compare("2")) {
      render_mode = "isometric";
    }
    if (!render_mode.compare("top-view")) {
      set.topview = true;
      set.oblique = false;
      set.isometric = false;
    } else if (!render_mode.compare("oblique")) {
      set.topview = false;
      set.oblique = true;
      set.isometric = false;
    } else if (!render_mode.compare("isometric")) {
      set.topview = false;
      set.oblique = false;
      set.isometric = true;
    } else {
      std::cerr << visible << std::endl;
      return 1;
    }
    if (!shadow_quality.compare("0")) {
      shadow_quality = "normal";
    } else if (!shadow_quality.compare("1")) {
      shadow_quality = "high";
    } else if (!shadow_quality.compare("2")) {
      shadow_quality = "ultra";
    }
    if (!shadow_quality.compare("normal")) {
      set.shadow_quality = false;
      set.shadow_quality_ultra = false;
    } else if (!shadow_quality.compare("high")) {
      set.shadow_quality = true;
      set.shadow_quality_ultra = false;
    } else if (!shadow_quality.compare("ultra")) {
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
    set.shadow_strength = shadow_strength;
    set.relief_strength = relief_strength;
    if (!rotation.compare("N")) {
      set.rotate = 1;
    } else if (!rotation.compare("W")) {
      set.rotate = 2;
    } else if (!rotation.compare("S")) {
      set.rotate = 3;
    } else if (!rotation.compare("E")) {
      set.rotate = 0;
    } else {
      std::cerr << visible << std::endl;
      return 1;
    }
    set.nightmode = vm.count("night-mode");
    set.heightmap = vm.count("height-map-mono") || vm.count("height-map-color");
    set.color = vm.count("height-map-color");
    if (!set.topview && (set.heightmap || set.color)) {
      std::cerr << "Height maps are only supported in top-view mode!"
                << std::endl;
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

    std::stringstream filename;
    filename << "world" << world_number
             << "_" << render_mode
             << "_sq-" << shadow_quality
             << "_sun-" << sun_direction
             << "_sstr-" << shadow_strength
             << "_rstr-" << relief_strength
             << "_rot-" << rotation;
    if (set.nightmode) {
      filename << "_night";
    }
    if (set.color) {
      filename << "_height-color";
    } else if (set.heightmap) {
      filename << "_height-mono";
    }
    filename << ".png";

    MinecraftWorld bf = world_number ? MinecraftWorld(world_number) :
                                       MinecraftWorld(av[1]);
    Renderer renderer(bf, set);

    std::pair<int, int> min_norm, max_norm;
    calculateMinMaxPoint(min_norm, max_norm, renderer);
    std::string buffer_file = "";

    size_t range = static_cast<size_t>(max_norm.second - min_norm.second + 1);
    boost::progress_display show_progress(range);
    std::list<std::vector<int> > tiles(range);
    size_t tiles_nr = fillTiles(tiles, renderer, min_norm, max_norm, show_progress);

    writeHeader(buffer_file, min_norm, max_norm, renderer);
    FILE* out_file = fopen(filename.str().c_str(), "wb");
    if (!out_file) {
      std::cerr << "Could not open output image file!" << std::endl;
      return 1;
    }

    tbb::atomic<size_t> progress_index;
    progress_index = 0;
    show_progress.restart(tiles_nr);
    std::list<std::vector<int> >::iterator it = tiles.begin();
    tbb::task_scheduler_init init;
    for (int i = min_norm.second; i <= max_norm.second; ++i) {
      tbb::parallel_for(tbb::blocked_range<std::vector<int>::iterator>
                                                         (it->begin(), it->end()),
                        ApplyFoo(&renderer, i, &progress_index, min_norm));
      ++it;
      show_progress += progress_index;
      progress_index = 0;
    }

    pamToPng(out_file);
  } catch (std::runtime_error e) {
    std::cerr << e.what() << std::endl;
    return 1;
  } catch (tbb::captured_exception e) {
    std::cerr << "In tbb loop: " << e.what() << e.name() << std::endl;
    return 1;
  }
  return 0;
}
