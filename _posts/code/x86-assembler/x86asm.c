// x86 Subset Assembler - Implementation (see API declared in x86asm.h)
//-----------------------------------------------------------------------------
// Copyright (C) 2017 Jeffrey L. Overbey.  Use of this source code is governed
// by a BSD-style license posted at http://blog.jeff.over.bz/license/

#include "x86asm.h"
#include <stddef.h>    // for NULL

uint8_t *nop(uint8_t *buf) {
        *buf++ = 0x90;
        return buf;
}

uint8_t *inc(reg32_t reg, uint8_t *buf) {
        *buf++ = 0x40 + reg;
        return buf;
}

uint8_t *dec(reg32_t reg, uint8_t *buf) {
        *buf++ = 0x48 + reg;
        return buf;
}

uint8_t *mov_immediate(reg32_t dest, int32_t value, uint8_t *buf) {
        *buf++ = 0xB8 + dest;
        *((int32_t *)buf) = value; buf += sizeof(int32_t);
        return buf;
}

uint8_t *mov_from_ptr(reg32_t dest, reg32_t src, uint8_t *buf) {
        *buf++ = 0x8B;
        if (src == ESP) {
                *buf++ = 8*dest + src;
                *buf++ = 0x24;
        } else if (src == EBP) {
                *buf++ = 0x45 + 8*dest;
                *buf++ = 0x00;
        } else {
                *buf++ = 8*dest + src;
        }
        return buf;
}

uint8_t *mov_to_ptr(reg32_t dest, reg32_t src, uint8_t *buf) {
        *buf++ = 0x89;
        if (dest == ESP) {
                *buf++ = 8*src + dest;
                *buf++ = 0x24;
        } else if (dest == EBP) {
                *buf++ = 0x45 + 8*src;
                *buf++ = 0x00;
        } else {
                *buf++ = 8*src + dest;
        }
        return buf;
}

#define DEFINE_INSN_RM(mnemonic, opcode)                     \
uint8_t *mnemonic(reg32_t dest, reg32_t src, uint8_t *buf) { \
        *buf++ = opcode;                                     \
        *buf++ = 8*dest + 0xC0 + src;                        \
        return buf;                                          \
}

DEFINE_INSN_RM( mov, 0x8B)
DEFINE_INSN_RM( add, 0x03)
DEFINE_INSN_RM( sub, 0x2B)
DEFINE_INSN_RM( and, 0x23)
DEFINE_INSN_RM(  or, 0x0B)
DEFINE_INSN_RM( xor, 0x33)
DEFINE_INSN_RM( cmp, 0x3B)

#define DEFINE_INSN_F7(mnemonic, reg_base)     \
uint8_t *mnemonic(reg32_t reg, uint8_t *buf) { \
        *buf++ = 0xF7;                         \
        *buf++ = reg_base + reg;               \
        return buf;                            \
}

DEFINE_INSN_F7( not, 0xD0)
DEFINE_INSN_F7( neg, 0xD8)
DEFINE_INSN_F7( mul, 0xE0)
DEFINE_INSN_F7(imul, 0xE8)
DEFINE_INSN_F7( div_, 0xF0)
DEFINE_INSN_F7(idiv, 0xF8)

uint8_t *cdq(uint8_t *buf) {
        *buf++ = 0x99;
        return buf;
}

#define DEFINE_INSN_D1C1(mnemonic, reg_base)                  \
uint8_t *mnemonic(reg32_t reg, uint8_t bits, uint8_t *buf) {  \
        switch (bits) {                                       \
        case 1: /* 1-bit shifts have a different opcode */    \
                *buf++ = 0xD1;                                \
                *buf++ = reg_base + reg;                      \
                break;                                        \
        default:                                              \
                *buf++ = 0xC1;                                \
                *buf++ = reg_base + reg;                      \
                *buf++ = bits;                                \
        }                                                     \
        return buf;                                           \
}                                                             \
uint8_t *mnemonic##_cl(reg32_t reg, uint8_t *buf) {           \
        *buf++ = 0xD3;                                        \
        *buf++ = reg_base + reg;                              \
        return buf;                                           \
}

DEFINE_INSN_D1C1(shl, 0xE0)
DEFINE_INSN_D1C1(shr, 0xE8)
DEFINE_INSN_D1C1(sar, 0xF8)

uint8_t *push(reg32_t reg, uint8_t *buf) {
        *buf++ = 0x50 + reg;
        return buf;
}

uint8_t *pop(reg32_t reg, uint8_t *buf) {
        *buf++ = 0x58 + reg;
        return buf;
}

uint8_t *call(reg32_t reg, uint8_t *buf) {
        *buf++ = 0xFF;
        *buf++ = 0xD0 + reg;
        return buf;
}

uint8_t *ret(uint16_t bytes, uint8_t *buf) {
        if (bytes == 0) {
                *buf++ = 0xC3;
        } else {
                *buf++ = 0xC2;
                *((uint16_t *)buf) = bytes; buf += sizeof(uint16_t);
        }
        return buf;
}

uint8_t *jmp(int32_t bytes, uint8_t *buf) {
        if (INT8_MIN <= bytes && bytes <= INT8_MAX) {
                *buf++ = 0xEB;
                *buf++ = (int8_t)bytes;
        } else {
                *buf++ = 0xE9;
                *((int32_t *)buf) = bytes; buf += sizeof(int32_t);
        }
        return buf;
}

#define DEFINE_INSN_JCC(mnemonic, byte_opcode)                     \
uint8_t *mnemonic(int32_t bytes, uint8_t *buf) {                   \
        if (INT8_MIN <= bytes && bytes <= INT8_MAX) {              \
                *buf++ = byte_opcode;                              \
                *buf++ = (int8_t)bytes;                            \
        } else {                                                   \
                *buf++ = 0x0F;                                     \
                *buf++ = byte_opcode + 0x10;                       \
                *((int32_t *)buf) = bytes; buf += sizeof(int32_t); \
        }                                                          \
        return buf;                                                \
}

DEFINE_INSN_JCC( jb, 0x72)
DEFINE_INSN_JCC(jae, 0x73)
DEFINE_INSN_JCC( je, 0x74)
DEFINE_INSN_JCC(jne, 0x75)
DEFINE_INSN_JCC(jbe, 0x76)
DEFINE_INSN_JCC( ja, 0x77)
DEFINE_INSN_JCC( jl, 0x7C)
DEFINE_INSN_JCC(jge, 0x7D)
DEFINE_INSN_JCC(jle, 0x7E)
DEFINE_INSN_JCC( jg, 0x7F)
