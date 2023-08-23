#ifndef UBC_H
#define UBC_H

#include <stdint.h>

// Status register flags (bit masks)
#define UBC_ST_ZERO 0x01
#define UBC_ST_OVFL 0x02 // Arithmetic overflow
#define UBC_ST_RES1 0x04 // Reserved for future use
#define UBC_ST_RES2 0x08 // ...
#define UBC_ST_RES3 0x10 // ...
#define UBC_ST_RES4 0x20 // ...
#define UBC_ST_RES5 0x40 // ...
#define UBC_ST_RES6 0x80 // ...

// Opcodes
#define UBC_OP_HLT     0x00
#define UBC_OP_CALL    0x01
// Jump operations
#define UBC_OP_JMP     0x02
#define UBC_OP_JMPi    0x03
#define UBC_OP_JZ      0x04
#define UBC_OP_JOV     0x05
#define UBC_OP_RES1    0x06
#define UBC_OP_RES2    0x07
// Math functions
#define UBC_OP_ADDI    0x08
#define UBC_OP_ADDF    0x09
#define UBC_OP_SUBI    0x0A
#define UBC_OP_SUBF    0x0B
#define UBC_OP_MULI    0x0C
#define UBC_OP_MULF    0x0D
#define UBC_OP_DIVI    0x0E
#define UBC_OP_DIVF    0x0F
// Stack functions
#define UBC_OP_PUSH8   0x10
#define UBC_OP_PUSH16  0x11
#define UBC_OP_PUSH32  0x12
#define UBC_OP_PUSH8i  0x13
#define UBC_OP_PUSH16i 0x14
#define UBC_OP_PUSH32i 0x15
#define UBC_OP_PUSHBP  0x16
#define UBC_OP_PUSHSP  0x17
#define UBC_OP_POP8    0x18
#define UBC_OP_POP16   0x19
#define UBC_OP_POP32   0x1A
#define UBC_OP_POPBP   0x1B
#define UBC_OP_POPSP   0x1C
// Comparison functions
#define UBC_OP_CMPI    0x1D
#define UBC_OP_CMPF    0x1E
#define UBC_OP_CMPB    0x1F

// TODO: Implement
typedef void ubcContext_t;

typedef struct ubcVM_s
{
    uint8_t  sr; // Status register
    uint32_t ip; // Instruction pointer
    void* memory;
} ubcVM_t;

#endif