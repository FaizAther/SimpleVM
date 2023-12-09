#ifndef __INST_H__
#define __INST_H__

typedef enum {
  MOV_LIT_REG         = 0x10,
  MOV_REG_REG         = 0x11,
  MOV_REG_MEM         = 0x12,
  MOV_MEM_REG         = 0x13,
  ADD_REG_REG         = 0x14,
  JMP_NOT_EQL         = 0x15,
  PUSH_LIT            = 0x16,
  PUSH_REG            = 0x17,
  POP                 = 0x18,
  CALL_LIT            = 0x19,
  CALL_REG            = 0x1A,
  RET                 = 0x1B,
  INSTRUCTION_COUNT   = 12,
} INSTRUCTION_T;

#endif //__INST_H__
