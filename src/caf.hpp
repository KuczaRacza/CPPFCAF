#include "types.hpp"
#include <string>
#include <string_view>
#include <vector>
class ArchiveFile {
public:
  ArchiveFile(std::string path);
  ~ArchiveFile();
  std::string path;
  std::vector<u64> content;
  std::string_view str;
  u64 file_size;
};
class CafParser {
public:
  CafParser();
  ~CafParser();
  auto meatdata() -> void;
  auto files() -> void;
  auto read_file(ArchiveFile &caf_file) -> void;
  auto dump_to_file(std::string dst) -> void;
  std::string raw_file;
  std::vector<ArchiveFile> caf_files;
  std::vector<std::string> caf_directories;
  static auto strToNum(std::string_view) -> u32;
  static auto Z64strToNum(std::string_view) -> u64;
  static auto u64_be_to_le(u64 be) -> u64;
  u64 header_end;
};
