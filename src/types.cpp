#include "types.hpp"
#include <cstdlib>
#include <stdlib.h>
#include <string.h>

void dstoffsetcopy(void *dst, void *src, u32 *offset, u32 size) {
  memcpy((u8 *)dst + *offset, src, size);
  *offset += size;
}
void srcoffsetcopy(void *dst, void *src, u32 *offset, u32 size) {
  memcpy(dst, (u8 *)src + *offset, size);
  *offset += size;
}

Stream::Stream() {}
Stream::Stream(u64 buffer_size) {
  ptr = (u8 *)malloc(buffer_size);
  size = buffer_size;
}
Stream::~Stream() { free(ptr); }