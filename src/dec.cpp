#define NDEBUG

#include "caf.hpp"
#include "types.hpp"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
auto read_file(std::string &file,const char *path) -> void {
  std::ifstream f;
  f.open(std::string(path), std::ios::binary | std::ios::in);
  if(!f.good()){
	  printf("cannot find file %s\n",path);
  }
  f.seekg(0, std::ios::seekdir::_S_end);
  u64 size = f.tellg();
  f.seekg(0, std::ios::seekdir::_S_beg);
  file.resize(size);
  f.read((char *)file.data(), size);
}
int main(int argc, char **argv) {
  CafParser caf;
  std::string path = "";
  bool save = true;
  bool decode = true;
  const std::string help =
      "Usage ./dec -i archive.caf\n options:\n --help - help\n --list - only "
      "lists files\n --dry-run does - not save\n";
  if (argc == 1) {
    printf("%s \n", help.c_str());
  }
  for (u32 i = 1; i < argc; i++) {
    if (strcmp("--help", argv[i]) == 0) {
      printf("%s \n", help.c_str());
      return 0;
    } else if (strcmp("--dry-run", argv[i]) == 0) {
      save = false;
    } else if (strcmp("--list", argv[i]) == 0) {
      decode = false;
    } else {
		if(path != ""){
			printf("Wrong option %s\n",argv[i]);
			exit(0);
		}
		else {
			path = argv[i];
		}
    }
  }
  read_file(caf.raw_file, path.c_str());
  caf.meatdata();
  if (decode) {
    caf.files();
    if (save) {
      std::string output_path = ".";
      caf.dump_to_file(output_path);
    }
  } else {
	  for (auto &file : caf.caf_files) {
		  printf("FILE: %s \n",file.path.c_str());
	  }
  }
  return 0;
}