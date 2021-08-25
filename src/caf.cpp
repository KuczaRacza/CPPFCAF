#include "caf.hpp"
#include "types.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <streambuf>
#include <string>
#include <string_view>
ArchiveFile::ArchiveFile(std::string path) { this->path = std::move(path); }
ArchiveFile::~ArchiveFile() {}
CafParser::CafParser() {}
CafParser::~CafParser() {}
auto CafParser::meatdata() -> void {
  // size of  strings
  constexpr u64 fw_size = sizeof("PLIK") - 1;
  constexpr u64 dw_size = sizeof("KATALOG") - 1;
  constexpr u64 vw_size = sizeof("CAF") - 1;
  constexpr u64 iw_size = sizeof("INDEKS") - 1;
  std::string_view file_data = raw_file;
  // everthing behind index is arledy parsed
  u64 index = 0;
  u64 filenumber = 0;
  // finds number of files
  {
    // Finds string "indeks"
    u64 in_pos = file_data.find("INDEKS");
    u64 endl_pos = file_data.find_first_of('\n', in_pos);
    // finds number written in polish
    u64 len = endl_pos - in_pos - iw_size - 1;
    // parse  words to number
    last_line.fill(file_data.substr(in_pos + iw_size + 1, len));
    filenumber = strToNum(last_line[0]);
    index = endl_pos + 1;
  }
  // alocates  memory to avoid many small alocations and realloactions
  caf_files.reserve(filenumber);

  std::string curr_path = "";
  // Parse  archive structure
  for (u64 i = 0; i < filenumber; i++) {
    // entery is file
    if (file_data.substr(index, fw_size) == "PLIK") {
      // crate emty file object sets name and  path and goes to next line
      u64 s_pos = index + fw_size + 1;
      u64 e_pos = file_data.find_first_of('\n', s_pos);
      index = e_pos + 1;
      caf_files.emplace_back(curr_path + raw_file.substr(s_pos, e_pos - s_pos));

    } else {
      // entery is directory
      // changes path  where next parsed files will be written
      // adds directory to list with directories to create
      // goes to next  line
      u64 s_pos = index + dw_size + 1;
      u64 e_pos = file_data.find_first_of('\n', s_pos);
      curr_path = "./" + raw_file.substr(s_pos, e_pos - s_pos) + "/";
      caf_directories.push_back(curr_path);
      index = e_pos + 1;
    }
  }
  // end of header makes  easier  to find first file
  header_end = index;
}

auto CafParser::files() -> void {
  constexpr u64 rw_size = sizeof("ROZMIAR ") - 1;
  std::string_view file_data = raw_file;
  std::string_view file_content =
      file_data.substr(header_end, raw_file.size() - header_end);
  if (file_content.find_first_of('X') == std::string_view::npos) {
    is_X_present = false;
  }
  u32 index = 0;
  // foreach file finds  entery "ROZMIAR"
  // files  alway beings with word  "ROZMIAR"
  for (auto &f : caf_files) {
    // finds end of file by finding next "ROZMIAR" or end of file
    u64 end = file_content.find_first_of("RO", index + rw_size) - 1;
    if (end == std::string_view::npos) [[unlikely]] {
      end = file_content.size() - 1;
    }
    // sets string to parse  in file   object
    f.str = file_content.substr(index, end - index);
    // parse  file
    read_file(f);
    index = end + 1;
  }
}
auto CafParser::read_file(ArchiveFile &caf_file) -> void {
  constexpr u64 rw_size = sizeof("ROZMIAR ") - 1;
  u64 size_endl = caf_file.str.find_first_of('\n');
  // parse size of file
  // important! real size  of file is alwas bigger due to padding
  // evry line  must have  number of bytes as multiply of 8
  // so eg.  file_size is  25B but real size with padding is 32B
  // parsed number must be converted to little endian.  File  format uses big
  // endian
  caf_file.file_size = u64_be_to_le(
      Z64strToNum(caf_file.str.substr(rw_size, size_endl - rw_size)).a);
  // reseve a lot of space  to avoid  reallocations
  caf_file.content.reserve(caf_file.file_size / 8 + 1);
  u64 index = size_endl + 1;
  // reads  line by line
  while (true) {
    u64 endl = caf_file.str.find_first_of('\n', index);
    std::string_view line = caf_file.str.substr(index, endl - index);
    // parse number and puts into vector
    u64_2 line_value = Z64strToNum(line);
    for (u32 i = 0; i < line_value.b; i++) {
      caf_file.content.push_back(line_value.a);
    }
    // ends  if read more bytes than filesize or there is no more lines
    if (endl == std::string_view::npos ||
        caf_file.content.size() * 8 >= caf_file.file_size) [[unlikely]] {
      break;
    }
    index = endl + 1;
  }
}
auto CafParser::dump_to_file(std::string dst) -> void {
  // first create directories to recrate archive structure
  // then  writes down content of vector to files
  std::filesystem::create_directories(dst);
  for (auto &drc : caf_directories) {
    std::filesystem::create_directories(dst + "/" + drc);
  }
  for (auto &flc : caf_files) {
    std::ofstream out_file(dst + "/" + (std::string)flc.path,
                           std::ios::binary | std::ios::out);
    out_file.write((char *)flc.content.data(), flc.file_size);
    out_file.close();
  }
}
auto CafParser::strToNum(std::string_view s) -> u32 {
  // parse polish words into numbers
  // very fast
  u32 number = 0;
  u64 index = 0;
  u64 second_index = 0;
  std::string_view str;
  // true if last word is beeing parsed
  bool parsed = false;

  u32 whitespaces = 0;

  while (true) {
    // splits  polish numebr into single words  eg "sto dwa" into "sto" and
    // "dwa"
    second_index = s.find(' ', index + 1);

    if (second_index == std::string::npos) [[unlikely]] {
      // string has only one word
      if (whitespaces == 0) {
        str = s.substr(index, s.size() - index);
      } else {
        // last word
        str = s.substr(index + 1, s.size() - index);
      }

      parsed = true;
    } else {
      // first word
      if (whitespaces == 0) [[unlikely]] {
        str = s.substr(index, second_index - index);
      } else {
        // evry next word
        str = s.substr(index + 1, second_index - index - 1);
      }
      whitespaces++;
    }

    u32 length = str.length();
    // first narrows down possible words using length
    // next finds  the word by comparing one letter
    switch (length) {
    case 3:
      switch (str[0]) {
      case 'd':
        number += 2;
        break;
      default:
        number += 100;
        break;
      }
      break;
    case 4:
      switch (str[0]) {
      case 'z':
        break;
      default:
        number += 3;
        break;
      }
      break;
    case 5:
      switch (str[0]) {
      case 'j':
        number += 1;
        break;
      default:
        number += 8;
        break;
      }

      break;

    case 6:
      switch (str[0]) {
      case 'c':
        number += 4;
        break;
      case 'p':
        number += 5;
        break;
      default:
        number += 7;
      }

      break;
    case 7:
      switch (str[2]) {
      case 'e':
        number += 6;
        break;
      case 'z':
        number += 300;
        break;
      }
      break;
    case 8:
      number += 800;
      break;
    case 9:
      switch (str[0]) {
      case 'd':
        number += 200;
        break;
      case 'c':
        break;
        number += 400;
        break;
      case 'p':
        number += 500;
        break;
      default:
        number += 700;
        break;
      }
      break;
    case 10:
      switch (str[4]) {
      case 's':
        number += 10;
        break;
      case 'w':
        number += 9;
        break;
      case 'a':
        number += 12;
        break;
      default:
        number += 600;
        break;
      }

      break;
    case 11:
      switch (str[0]) {
      case 'j':
        number += 11;
        break;
      case 't':
        number += 13;
        break;
      default:
        number += 16;
        break;
      }
      break;

    case 12:
      switch (str[0]) {
      case 'c':
        number += 14;
        break;
      case 'o':
        number += 18;
        break;
      case 'd':
        number += 20;
        break;
      case 't':
        number += 30;
        break;
      default:
        number += 15;
        break;
      }
      break;

    case 13:
      switch (str[0]) {

      case 's':
        number += 17;
        break;
      case 'c':
        number += 40;
        break;
      default:
        number += 900;
      }
      break;
    case 14:
      number += 80;
      break;
    case 15:
      switch (str[0]) {
      case 'p':
        number += 50;
        break;
      default:
        number += 70;
        break;
      }
      break;

    case 16:
      switch (str[0]) {
      case 'd':
        number += 19;
        break;
      default:
        number += 60;
        break;
      }
      break;

    case 19:
      number += 90;
      break;
    default:
      std::cout << str << '\n';
      break;
    }
    if (parsed) [[unlikely]] {
      break;
    }
    index = second_index;
  }
  return number;
}
auto CafParser::Z64strToNumBitshift(std::string_view str) -> u64 {
  // parse z64number
  // splits string
  // parse evry polish number
  // does bitshifting
  u64 separators = 0;
  u64 separator_index = 0;
  while (true) {
    separator_index += 2;
    separator_index = str.find_first_of('<', separator_index);
    if (separator_index != std::string_view::npos) [[likely]] {
      separators++;
    } else {
      break;
    }
  }
  u64 outnumber = 0;
  u64 index = 0;
  for (u32 i = 0; i < separators; i++) {
    u64 end_index = str.find_first_of('<', index);
    u8 num = strToNum(str.substr(index, end_index - index));
    index = end_index + 2;
    outnumber = (outnumber >> 8) | ((u64)num << 56);
  }
  u8 num = strToNum(str.substr(index, str.size() - index + 2));
  outnumber = (outnumber >> 8) | ((u64)num << 56);
  return outnumber;
}
auto CafParser::Z64strToNum(std::string_view str) -> u64_2 {
  // parse lines
  // looks for X oprator
  // if present splits string
  // and parse separetly
  u64_2 ret;
  // cache lookup
  // compares string size
  // and if match compare charachters
  // jumps  3 characters forward bc in polish shotrest word for number have 3
  // characters
  for (u32 i = 0; i < last_line.size(); i++) {
    if (last_line[(cache_miss + i) % last_line.size()].size() == str.size()) {
      bool equal = true;
      for (u32 j = 0; j < str.size(); j += 3) {
        if (str[j] != last_line[(cache_miss + i) % last_line.size()][j]) {
          equal = false;
          break;
        }
      }
      if (equal) {
        return last_value[(cache_miss + i) % last_line.size()];
      }
    }
  }
  u64 reapat_separator;
  // X mode
  // find is faster in bulk
  // so druing initial parsing is determined if
  // X is prsent
  // if not present
  // looking for X is skipped
  if (is_X_present) {
    reapat_separator = str.find_first_of('X');
  } else {
    reapat_separator = std::string_view::npos;
  }

  if (reapat_separator == std::string_view::npos) [[likely]] {
    // b - number of repeats
    // a- byte
    ret.b = 1;
    ret.a = Z64strToNumBitshift(str);
  } else {
    std::string_view byte_str = str.substr(0, reapat_separator);
    std::string_view repeat_str =
        str.substr(reapat_separator + 2, str.size() - reapat_separator - 1);
    ret.a = Z64strToNumBitshift(byte_str);
    ret.b = u64_be_to_le(Z64strToNumBitshift(repeat_str));
  }

  last_line[cache_miss % last_line.size()] = str;
  last_value[cache_miss % last_line.size()] = ret;
  cache_miss++;
  return ret;
}
auto CafParser::u64_be_to_le(u64 be) -> u64 {
  // changes endian
  u64 le = 0;
  u8 *big = (u8 *)&be;
  for (u32 i = 0; i < 8; i += 1) {
    le = (le << 8) | ((u64)(big[i]));
  }
  return le;
}
