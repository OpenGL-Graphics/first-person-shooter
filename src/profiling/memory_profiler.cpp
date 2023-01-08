#include <stdio.h>

#include "profiling/memory_profiler.hpp"

/**
 * For an explanation of extracted VmSize, see: https://stackoverflow.com/a/17190631/2228912
 * Memory size in /proc/self/statm returned in pages (1 page = 4kb = 4096 bytes)
 *
 * NOTE: Not practical to monitor as mem usage same before & after certain instructions => use valgrind/massif
 */
void MemoryProfiler::profile(const std::string& label) {
  FILE* f = fopen("/proc/self/statm", "r");
  long vmsize_pages, vmrss_pages;
  fscanf(f, "%ld %ld", &vmsize_pages, & vmrss_pages);
  fclose(f);

  const int size_page_kb = 4;
  long vmsize_kb = vmsize_pages * size_page_kb;
  long vmrss_kb = vmrss_pages * size_page_kb;
  printf("%s: reachable = %ld kb, used = %ld kb \n", label.c_str(), vmsize_kb, vmrss_kb);
}
