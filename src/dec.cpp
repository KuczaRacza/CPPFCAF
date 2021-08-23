#include "caf.hpp"
#include "types.hpp"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
auto read_file(std::string &file, char *path) -> void {
  std::ifstream f;
  f.open(std::string(path), std::ios::binary | std::ios::in);
  f.seekg(0, std::ios::seekdir::_S_end);
  u64 size = f.tellg();
  f.seekg(0, std::ios::seekdir::_S_beg);
  file.resize(size);
  f.read((char *)file.data(), size);
}
int main(int argc, char **argv) {
  CafParser caf;
  read_file(caf.raw_file, argv[1]);
  caf.meatdata();
  caf.files();
  caf.dump_to_file(argv[1]);


  return 0;
}