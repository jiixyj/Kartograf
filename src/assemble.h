#ifndef _ASSEMBLE_H
#define _ASSEMBLE_H

#include <boost/progress.hpp>
#include <tbb/atomic.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>

#include "./nbt.h"

std::pair<int, int> projectCoords(std::pair<int, int> p, int phi);
uint16_t writeHeader(std::string filename,
                   std::pair<int, int> min_norm,
                   std::pair<int, int> max_norm,
                   const nbt& bf);
class ApplyFoo {
  nbt* bf_;
  int i_;
  tbb::atomic<size_t>* index_;
  std::pair<int, int> min_norm_;
 public:
  void operator() (const tbb::blocked_range<std::vector<int>::iterator>& r) const;
  ApplyFoo(nbt* bf, int i, tbb::atomic<size_t>* index,
           std::pair<int, int> min_norm);
 private:
  ApplyFoo& operator=(const ApplyFoo&);
};

Settings getSettings();
void calculateMinMaxPoint(std::pair<int, int>& min_norm,
                          std::pair<int, int>& max_norm,
                          const nbt& bf);
void pamToPng(FILE* out_file);
size_t fillTiles(std::list<std::vector<int> >& tiles, const nbt& bf,
                 const std::pair<int, int>& min_norm,
                 const std::pair<int, int>& max_norm,
                 boost::progress_display& show_progress);


#endif  // _ASSEMBLE_H
