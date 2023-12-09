#include "cpu.hpp"
#include "memory.hpp"
#include <iostream>
#include "registers.hpp"
#include "instructions.hpp"
#include <assert.h>

int
main(int argc, char *argv[])
{
  CPU<256 * 256> cpu;
  cpu.description();

  uint16_t i = 0;
  cpu.memory.region[i++] = INSTRUCTION_T::MOV_LIT_REG;
  cpu.memory.region[i++] = 0x12;
  cpu.memory.region[i++] = 0x34;
  cpu.memory.region[i++] = Registers::R1;
  cpu.step();
  cpu.debug();
  assert(cpu.getRegister(Registers::R1) == 0x1234);

  cpu.memory.region[i++] = INSTRUCTION_T::MOV_LIT_REG;
  cpu.memory.region[i++] = 0x56;
  cpu.memory.region[i++] = 0x78;
  cpu.memory.region[i++] = Registers::R2;
  cpu.step();
  cpu.debug();
  assert(cpu.getRegister(Registers::R2) == 0x5678);

  cpu.memory.region[i++] = INSTRUCTION_T::ADD_REG_REG; // add inst
  cpu.memory.region[i++] = Registers::R1; // r1
  cpu.memory.region[i++] = Registers::R2; // r2
  cpu.step();
  cpu.debug();
  assert(cpu.getRegister(Registers::ACC) == 0x68ac);

  cpu.memory.region[i++] = INSTRUCTION_T::MOV_REG_MEM; // mov ACC addr :: [addr] = [ACC]
  cpu.memory.region[i++] = Registers::ACC; // r1
  cpu.memory.region[i++] = 0x01;
  cpu.memory.region[i++] = 0x00;
  cpu.step();
  cpu.debug();
  cpu.viewMemoryAt(0x0100);
  uint16_t val = (cpu.memory.region[0x0100] << 8) | (cpu.memory.region[0x0101]);
  assert(val == 0x68ac);

  // i = 0; do { ++i; } while (i != 5);

  // [r1] = 1
  cpu.memory.region[i++] = INSTRUCTION_T::MOV_LIT_REG;
  cpu.memory.region[i++] = 0x00;
  cpu.memory.region[i++] = 0x01;
  cpu.memory.region[i++] = Registers::R1;
  cpu.step();
  cpu.debug();
  assert(cpu.getRegister(Registers::R1) == 0x0001);

  // [r2] = 1
  cpu.memory.region[i++] = INSTRUCTION_T::MOV_LIT_REG;
  cpu.memory.region[i++] = 0x00;
  cpu.memory.region[i++] = 0x01;
  cpu.memory.region[i++] = Registers::ACC;
  cpu.step();
  cpu.debug();
  assert(cpu.getRegister(Registers::ACC) == 0x0001);

  // [acc] = [r1] + [acc]
  uint16_t jumpAddr = i;
  cpu.memory.region[i++] = INSTRUCTION_T::ADD_REG_REG; // add inst
  cpu.memory.region[i++] = Registers::R1; // r1
  cpu.memory.region[i++] = Registers::ACC; // ACC
  cpu.step();
  cpu.debug();
  assert(cpu.getRegister(Registers::ACC) == 0x0002);

  cpu.memory.region[i++] = INSTRUCTION_T::JMP_NOT_EQL;
  cpu.memory.region[i++] = 0x00;
  cpu.memory.region[i++] = 0x05;
  cpu.memory.region[i++] = jumpAddr & 0xFF00;
  cpu.memory.region[i++] = jumpAddr & 0x00FF;

  for (uint16_t i = 0x0003; i <= 0x0005; ++i) {
    cpu.step();
    cpu.debug();
    assert(cpu.getRegister(Registers::IP) == jumpAddr);
    cpu.step();
    cpu.debug();
    assert(cpu.getRegister(Registers::ACC) == i);
  }
  cpu.step();
  cpu.debug();
  cpu.viewMemoryAt(cpu.getRegister(Registers::IP) - 8);
  cpu.viewMemoryAt(cpu.getRegister(Registers::IP));
  assert(cpu.getRegister(Registers::IP) != jumpAddr);

  cpu.memory.region[i++] = 0x16; // push-lit
  cpu.memory.region[i++] = 0x01;
  cpu.memory.region[i++] = 0x02;

  unsigned short sp_save = cpu.getRegister(Registers::SP);
  cpu.step();
  cpu.debug();
  cpu.viewMemoryAt(cpu.getRegister(Registers::SP));
  assert(cpu.getRegister(Registers::SP) == sp_save - 2);
  assert(cpu.memory.region[sp_save] == 0x01);
  assert(cpu.memory.region[sp_save+1] == 0x02);

  cpu.memory.region[i++] = 0x18; // pop
  cpu.memory.region[i++] = Registers::R1;

  sp_save = cpu.getRegister(Registers::SP);
  unsigned char val1 = cpu.memory.region[sp_save+2];
  unsigned char val2 = cpu.memory.region[sp_save+3];
  cpu.step();
  cpu.debug();
  cpu.viewMemoryAt(0x5555);
  assert(cpu.getRegister(Registers::SP) == sp_save + 2);
  unsigned short saved_reg = cpu.getRegister(Registers::R1);
  assert((saved_reg&0x00FF) == val1);
  assert(((saved_reg&0xFF00)>>8) == val2);

  cpu.setRegister(Registers::R2, 0x5678); // R2 = 0x5678

  cpu.memory.region[i++] = 0x17; // push-reg
  cpu.memory.region[i++] = Registers::R2;

  sp_save = cpu.getRegister(Registers::SP);
  cpu.step();
  cpu.debug();
  cpu.viewMemoryAt(cpu.getRegister(Registers::SP));
  assert(cpu.getRegister(Registers::SP) == sp_save - 2);
  assert(cpu.memory.region[sp_save] == ((cpu.getRegister(Registers::R2) & 0xFF00) >> 8));
  assert(cpu.memory.region[sp_save+1] == (cpu.getRegister(Registers::R2) & 0x00FF));

  cpu.memory.region[i++] = 0x18; // pop
  cpu.memory.region[i++] = Registers::R1;

  sp_save = cpu.getRegister(Registers::SP);
  val1 = cpu.memory.region[sp_save+2];
  val2 = cpu.memory.region[sp_save+3];
  cpu.step();
  cpu.debug();
  cpu.viewMemoryAt(sp_save);
  assert(cpu.getRegister(Registers::SP) == sp_save + 2);
  saved_reg = cpu.getRegister(Registers::R1);
  assert((saved_reg&0x00FF) == val1);
  assert(((saved_reg&0xFF00)>>8) == val2);

  return 0;
}