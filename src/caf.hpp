#include "types.hpp"
#include <array>
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
  u64 cache_miss = 0;
  std::array<std::string_view, 18> last_line;
  std::array<u64_2, 18> last_value;
  std::vector<ArchiveFile> caf_files;
  std::vector<std::string> caf_directories;
  auto strToNum(std::string_view) -> u32;
  auto Z64strToNum(std::string_view) -> u64_2;
  auto Z64strToNumBitshift(std::string_view) -> u64;
  auto u64_be_to_le(u64 be) -> u64;
  u64 header_end;
  bool is_X_present = true;
};
