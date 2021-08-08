#ifndef FILE_HPP
#define FILE_HPP

#include <string>
#include <vector>

/* Wrapper around std::ifstream */
namespace File {
  std::string get_content(const std::string& path);
  std::vector<std::vector<char>> get_chars(const std::string& path);
};

#endif // FILE_HPP
