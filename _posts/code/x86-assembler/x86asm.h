// x86 Subset Assembler - API
//-----------------------------------------------------------------------------
// Copyright (C) 2017 Jeffrey L. Overbey.  Use of this source code is governed
// by a BSD-style license posted at http://blog.jeff.over.bz/license/

#ifndef X86ASM_H
#define X86ASM_H

#include <stdint.h> // uint8_t, unint32_t

typedef enum { EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI } reg32_t;

uint8_t *nop(uint8_t *buf);

uint8_t *mov_immediate(reg32_t dest, int32_t value, uint8_t *buf);
uint8_t * mov_from_ptr(reg32_t dest, reg32_t src, uint8_t *buf);
uint8_t *   mov_to_ptr(reg32_t dest, reg32_t src, uint8_t *buf);
uint8_t *          mov(reg32_t dest, reg32_t src, uint8_t *buf);

uint8_t *  add(reg32_t dest, reg32_t src, uint8_t *buf);
uint8_t *  sub(reg32_t dest, reg32_t src, uint8_t *buf);
uint8_t *  and(reg32_t dest, reg32_t src, uint8_t *buf);
uint8_t *   or(reg32_t dest, reg32_t src, uint8_t *buf);
uint8_t *  xor(reg32_t dest, reg32_t src, uint8_t *buf);
uint8_t *  cmp(reg32_t dest, reg32_t src, uint8_t *buf);
uint8_t *  inc(reg32_t reg, uint8_t *buf);
uint8_t *  dec(reg32_t reg, uint8_t *buf);
uint8_t *  not(reg32_t reg, uint8_t *buf);
uint8_t *  neg(reg32_t reg, uint8_t *buf);
uint8_t *  mul(reg32_t reg, uint8_t *buf);
uint8_t * imul(reg32_t reg, uint8_t *buf);
uint8_t * div_(reg32_t reg, uint8_t *buf);
uint8_t * idiv(reg32_t reg, uint8_t *buf);
uint8_t *  cdq(uint8_t *buf);

uint8_t *   shl(reg32_t reg, uint8_t bits, uint8_t *buf);
uint8_t *shl_cl(reg32_t reg, uint8_t *buf);
uint8_t *   shr(reg32_t reg, uint8_t bits, uint8_t *buf);
uint8_t *shr_cl(reg32_t reg, uint8_t *buf);
uint8_t *   sar(reg32_t reg, uint8_t bits, uint8_t *buf);
uint8_t *sar_cl(reg32_t reg, uint8_t *buf);

uint8_t *push(reg32_t reg, uint8_t *buf);
uint8_t * pop(reg32_t reg, uint8_t *buf);
uint8_t *call(reg32_t reg, uint8_t *buf);
uint8_t * ret(uint16_t bytes, uint8_t *buf);

uint8_t * jmp(int32_t relative_bytes, uint8_t *buf);
uint8_t *  jb(int32_t relative_bytes, uint8_t *buf);
uint8_t * jae(int32_t relative_bytes, uint8_t *buf);
uint8_t *  je(int32_t relative_bytes, uint8_t *buf);
uint8_t * jne(int32_t relative_bytes, uint8_t *buf);
uint8_t * jbe(int32_t relative_bytes, uint8_t *buf);
uint8_t *  ja(int32_t relative_bytes, uint8_t *buf);
uint8_t *  jl(int32_t relative_bytes, uint8_t *buf);
uint8_t * jge(int32_t relative_bytes, uint8_t *buf);
uint8_t * jle(int32_t relative_bytes, uint8_t *buf);
uint8_t *  jg(int32_t relative_bytes, uint8_t *buf);

#endif
