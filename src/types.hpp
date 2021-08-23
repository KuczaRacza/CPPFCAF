#pragma once
#include <stdint.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
class Stream {
public:
  Stream();
  Stream(u64 buffer_size);
  ~Stream();
  u8 *ptr;
  u64 size;
};
void dstoffsetcopy(void *dst, void *src, u32 *offset, u32 size);
void srcoffsetcopy(void *dst, void *src, u32 *offset, u32 size);
