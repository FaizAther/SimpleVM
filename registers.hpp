#ifndef __REG_H__
#define __REG_H__

#include <string_view>
#include <array>

#define REGISTER_RANGE                                    \
{                                                         \
  Registers::register_t::IP, Registers::register_t::ACC,  \
  Registers::register_t::R1, Registers::register_t::R2,   \
  Registers::register_t::R3, Registers::register_t::R4,   \
  Registers::register_t::R5, Registers::register_t::R6,   \
  Registers::register_t::R7, Registers::register_t::R8,   \
  Registers::register_t::SP, Registers::register_t::FP,   \
}

class Registers {
public:
  typedef enum {
    IP, ACC,
    R1, R2, R3, R4,
    R5, R6, R7, R8,
    SP, FP,
    REGISTER_COUNT
  } register_t;

  typedef std::string_view registerName_t;

  static constexpr std::array<registerName_t, REGISTER_COUNT> registerNames = {
     "ip", "acc",
     "r1", "r2", "r3", "r4",
     "r5", "r6", "r7", "r8",
     "sp", "fp",
  };

  static registerName_t registerName(register_t inst) {
    return registerNames[inst];
  }
};

#endif // REG_H