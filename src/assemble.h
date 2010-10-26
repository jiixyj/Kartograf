#ifndef _ASSEMBLE_H
#define _ASSEMBLE_H

#include <tbb/parallel_for.h>
#include <boost/progress.hpp>

#include "./image.h"
#include "./nbt.h"

std::pair<int, int> projectCoords(std::pair<int, int> p, int phi);
int render_tile(std::string filename,
                Image<uint8_t>& image,
                std::pair<int, int> projected,
                uint16_t header_size);
uint16_t writeHeader(std::string filename,
                   std::pair<int, int> min_norm,
                   std::pair<int, int> max_norm,
                   uint32_t& width, uint32_t& height,
                   const nbt& bf);
class ApplyFoo {
  nbt* bf_;
  int i_;
  tbb::atomic<size_t>* index_;
  std::pair<int, int> min_norm_;
 public:
  void operator() (const tbb::blocked_range<std::vector<int>
                                                          ::iterator>& r) const;
  ApplyFoo(nbt* bf, int i, tbb::atomic<size_t>* index,
           std::pair<int, int> min_norm);
 private:
  ApplyFoo& operator=(const ApplyFoo&);
};

Settings getSettings();
void calculateMinMaxPoint(std::pair<int, int>& min_norm,
                          std::pair<int, int>& max_norm,
                          const nbt& bf);
void pamToPng(std::string pam_name, std::string png_name, uint16_t header_size,
              uint32_t width, uint32_t height);
size_t fillTiles(std::list<std::vector<int> >& tiles, const nbt& bf,
                 const std::pair<int, int>& min_norm,
                 const std::pair<int, int>& max_norm,
                 boost::progress_display& show_progress);


#endif  // _ASSEMBLE_H
