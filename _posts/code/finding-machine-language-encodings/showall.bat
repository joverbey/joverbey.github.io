@echo off
REM Shows machine language encodings of all instructions supported by the
REM x86 subset assembler.  Requires showinsn.bat.

set immeds=0 1 0Fh 0F0h 12345678h 0FFFFFFFEh 0FFFFFFFFh
set regs32=eax ecx edx ebx esp ebp esi edi

call showinsn nop

for %%r in (%regs32%) do (
	for %%n in (%immeds%) do (
		call showinsn "mov %%r, %%n"
	)
)

for %%r in (%regs32%) do (
	for %%s in (%regs32%) do (
		call showinsn "mov %%r, DWORD PTR [%%s]"
	)
)

for %%r in (%regs32%) do (
	for %%s in (%regs32%) do (
		call showinsn "mov DWORD PTR [%%s], DWORD PTR %%r"
	)
)

for %%i in (mov add sub and or xor cmp) do (
	for %%r in (%regs32%) do (
		for %%s in (%regs32%) do (
			call showinsn "%%i %%r, %%s"
		)
	)
)

for %%i in (inc dec not neg mul imul div idiv) do (
	for %%r in (%regs32%) do (
		call showinsn "%%i %%r"
	)
)

call showinsn cdq

for %%i in (shl shr sar) do (
	for %%r in (%regs32%) do (
		call showinsn "%%i %%r, cl"
	)
)

for %%i in (shl shr sar) do (
	for %%r in (%regs32%) do (
		for %%n in (0 1 2 3 32 64 65 255) do (
			call showinsn "%%i %%r, %%n"
		)
	)
)

for %%i in (push pop call) do (
	for %%r in (%regs32%) do (
		call showinsn "%%i %%r"
	)
)

for %%n in (0 1 2 4 16 256) do (
	call showinsn "ret %%n"
)

for %%i in (jmp jb jae je jne jbe ja jl jge jle jg) do (
	for %%l in (back zero l5 l10 l100 l200 l300) do (
		call showinsn "%%i %%l"
	)
)

REM ---------------------------------------------------------------------------
REM Copyright (C) 2017 Jeffrey L. Overbey.  Use of this source code is governed
REM by a BSD-style license posted at http://blog.jeff.over.bz/license/
