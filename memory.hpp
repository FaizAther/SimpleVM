#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <array>
#include <bits/stdint-uintn.h>

template <int MEM_SIZE_IN_BYTES, typename T = uint8_t>
class Memory {
public:
  std::array<T, MEM_SIZE_IN_BYTES> region;
};

#endif // __MEMORY_H__