#include <stdio.h>

#include "profiling/memory_profiler.hpp"

/**
 * For an explanation of extracted VmSize, see: https://stackoverflow.com/a/17190631/2228912
 * Memory size in /proc/self/statm returned in pages (1 page = 4kb = 4096 bytes)
 */
void MemoryProfiler::profile(const std::string& label) {
  FILE* f = fopen("/proc/self/statm", "r");
  long vmsize_pages;
  fscanf(f, "%ld", &vmsize_pages);
  fclose(f);

  const int size_page_kb = 4;
  long vmsize_kb = vmsize_pages * size_page_kb;
  printf("%s: %ld kb\n", label.c_str(), vmsize_kb);
}
