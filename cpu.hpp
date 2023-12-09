
#ifndef __CPU_H__
#define __CPU_H__

#include "memory.hpp"
#include <string>
#include <iostream>
#include <map>
#include "registers.hpp"
#include "instructions.hpp"

template<
  unsigned int CPU_MEMORY_SIZE,
  typename registerValue_t = uint16_t,
  typename instructionValue_t = uint8_t,
  typename instructionDoubleValue_t = uint16_t
>
class CPU {
public:
  CPU() {
    std::cout << "CPU init" << std::endl;
    setRegister(Registers::SP, CPU_MEMORY_SIZE - 1 - 1);
    setRegister(Registers::FP, CPU_MEMORY_SIZE - 1 - 1);
  }
  Memory<CPU_MEMORY_SIZE, instructionValue_t> memory = {0};
  Memory<Registers::register_t::REGISTER_COUNT,registerValue_t> registers = {0};
  void setRegister(Registers::register_t reg, registerValue_t val) {
    registers.region[reg] = val;
  }
  registerValue_t getRegister(Registers::register_t reg) {
    return registers.region[reg];
  }
  instructionValue_t fetch() {
    const registerValue_t nextInstructionAddress = getRegister(Registers::register_t::IP);
    const instructionValue_t instruction = memory.region[nextInstructionAddress];
    setRegister(Registers::IP, nextInstructionAddress + 1);
    return instruction;
  }
  instructionDoubleValue_t fetchDouble() {
    const registerValue_t nextInstructionAddress = getRegister(Registers::register_t::IP);
    const instructionDoubleValue_t instruction =
      (((instructionDoubleValue_t)memory.region[nextInstructionAddress]) << (sizeof(instructionValue_t) * 8)) | \
      memory.region[nextInstructionAddress + 1];
    setRegister(Registers::IP, nextInstructionAddress + 2);
    return instruction;
  }


  inline void push(instructionDoubleValue_t addr, instructionDoubleValue_t val) {
    memory.region[addr+1] = (instructionValue_t)(val & 0x00FF);
    memory.region[addr] = (instructionValue_t)((val & 0xFF00) >> 8);
    setRegister(Registers::SP, addr-2);
  }

  inline void pop(instructionDoubleValue_t reg) {
    registerValue_t addr_from = getRegister(Registers::SP);
    instructionDoubleValue_t val = memory.region[addr_from+2] | (memory.region[addr_from + 3] << 8);
    setRegister((Registers::register_t)reg, val);
    setRegister(Registers::SP, addr_from+2);
  }

  void execute(instructionValue_t inst) {
    switch (inst) {
      // move val reg :: [reg] = val
      case INSTRUCTION_T::MOV_LIT_REG: {
        instructionDoubleValue_t val = fetchDouble();
        registerValue_t reg = fetch() % Registers::REGISTER_COUNT;
        setRegister((Registers::register_t)reg, val);
        break;
      }
      // move r1 r2 :: [r2] = [r1]
      case INSTRUCTION_T::MOV_REG_REG: {
        registerValue_t regFrom = fetch() % Registers::REGISTER_COUNT;
        registerValue_t regTo = fetch() % Registers::REGISTER_COUNT;
        registerValue_t val = getRegister((Registers::register_t)regFrom);
        setRegister((Registers::register_t)regTo , val);
        break;
      }
      // move reg addr :: [addr] = [reg]
      case INSTRUCTION_T::MOV_REG_MEM: {
        registerValue_t regFrom = fetch() % Registers::REGISTER_COUNT;
        instructionDoubleValue_t addr = fetchDouble();
        registerValue_t val = getRegister((Registers::register_t)regFrom);
        memory.region[addr] = (instructionValue_t)((val & 0xFF00) >> 8);
        memory.region[addr+1] = (instructionValue_t)(val & 0x00FF);
        break;
      }
      // move addr reg :: [reg] = [addr]
      case INSTRUCTION_T::MOV_MEM_REG: {
        instructionDoubleValue_t addr = fetchDouble();
        registerValue_t regTo = fetch() % Registers::REGISTER_COUNT;
        registerValue_t val = (memory.region[addr] << 8) | memory.region[addr+1];
        setRegister((Registers::register_t)regTo , val);
        break;
      }
      // acc = add r1 r2
      case INSTRUCTION_T::ADD_REG_REG: {
        instructionValue_t r1 = fetch(), r2 = fetch();
        instructionDoubleValue_t v1 = getRegister((Registers::register_t)r1), v2 = getRegister((Registers::register_t)r2);
        setRegister(Registers::ACC, v1 + v2);
        break;
      }
      // jne val addr :: if val != [ACC] then goto addr
      case INSTRUCTION_T::JMP_NOT_EQL: {
        instructionDoubleValue_t val = fetchDouble();
        instructionDoubleValue_t addr = fetchDouble();
        registerValue_t accVal = getRegister(Registers::ACC);
        if (val != accVal) {
          setRegister(Registers::IP, addr);
        }
        break;
      }
      case INSTRUCTION_T::PUSH_LIT: {
        instructionDoubleValue_t val = fetchDouble();
        registerValue_t addr = getRegister(Registers::SP);
        // memory.region[addr+1] = (instructionValue_t)(val & 0x00FF);
        // memory.region[addr] = (instructionValue_t)((val & 0xFF00) >> 8);
        // setRegister(Registers::SP, addr-2);
        push(addr, val);
        break;
      } case INSTRUCTION_T::PUSH_REG: {
        instructionValue_t reg = fetch() % Registers::REGISTER_COUNT;
        instructionDoubleValue_t val = getRegister((Registers::register_t)reg);
        instructionDoubleValue_t addr = getRegister(Registers::SP);
        // memory.region[addr+1] = (instructionValue_t)(val & 0x00FF);
        // memory.region[addr] = (instructionValue_t)((val & 0xFF00) >> 8);
        // setRegister(Registers::SP, addr-2);
        push(addr, val);
        break;
      } case INSTRUCTION_T::POP: {
        instructionDoubleValue_t reg = fetch() % Registers::REGISTER_COUNT;
        // registerValue_t addr_from = getRegister(Registers::SP);
        // instructionDoubleValue_t val = memory.region[addr_from+2] | (memory.region[addr_from + 3] << 8);
        // setRegister((Registers::register_t)reg, val);
        // setRegister(Registers::SP, addr_from+2);
        pop(reg);
        break;
      } case INSTRUCTION_T::CALL_LIT: {
        const instructionDoubleValue_t addr = fetchDouble();
        
        break;
      } case INSTRUCTION_T::CALL_REG: {
        break;
      } case INSTRUCTION_T::RET: {
        break;
      } default: {
        printf("ERR: INST{0x%02x} NOT IMPLEMENTED\n", inst);
      }
    }
  }

  void step(void) {
    execute(fetch());
  }

  void debug(void) {
    std::cout << "CPU {" << std::endl;
    for (const auto &reg : REGISTER_RANGE) {
      std::cout << "\t" << Registers::registerName(reg) << "\t:: " << std::hex << "0x"<< getRegister(reg) << std::endl;
    }
    std::cout << "}" << std::endl;
  }

  void description() {
    printf("CPU{ "
      "\tRegister Size\t{%lu}"
      "\tInstruction Size\t{%lu}"
      "\tMemory Size\t{%u}"
      "\tRegister Count\t{%u}"
      " }\n"
      , sizeof(registerValue_t)
      , sizeof(instructionValue_t)
      , CPU_MEMORY_SIZE
      , Registers::REGISTER_COUNT
    );
    debug();
  }

  void viewMemoryAt(instructionDoubleValue_t addr) {
    // 0x11AA: 0x04 0x04 0x04 0x04 0x04 0x04 0x04 0x04
    printf("Memory{\n\t%04x: ", addr);
    for (uint8_t i = 0; i < 8 && addr + i < CPU_MEMORY_SIZE; ++i) {
      printf("%02x ", memory.region[addr +i]);
    }
    printf("\n}\n");
  }
};

#endif // __CPU_H__