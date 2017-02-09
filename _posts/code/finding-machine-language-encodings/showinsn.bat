@echo off

REM Displays the machine language encoding of an x86 instruction by assembling
REM a small program using the Microsoft Macro Assembler, then disassembling the
REM resulting object file using DUMPBIN.
REM
REM For jump instructions, there are seven labels to jump to:
REM
REM - "back" is the start of the jump.
REM   The encoded instruction will be a backward jump by the number of bytes
REM   in the jump instruction (probably 2).
REM
REM - "zero" is the instruction immediately after the jump.
REM   The encoded instruction will be a 0-byte jump.
REM
REM - "l5", "l10", "l100", "l200", and "l300" are 5, 10, 100, 200, and 300
REM   bytes after the jump, repectively.

if "%~1" == "" (
	echo Example Usage: showinsn "mov eax, ebx"
	exit /B 1
)

REM %~1 is the first command line argument with double-quotes removed.
REM This is important below since we do not want the quotes in the .asm file.

echo .model flat             > temp.asm
echo .code                  >> temp.asm
echo main PROC              >> temp.asm
echo back: %~1              >> temp.asm
echo zero: nop              >> temp.asm
echo       nop              >> temp.asm
echo       nop              >> temp.asm
echo       nop              >> temp.asm
echo       nop              >> temp.asm
echo   l5: nop              >> temp.asm
echo       nop              >> temp.asm
echo       nop              >> temp.asm
echo       nop              >> temp.asm
echo       nop              >> temp.asm
echo  l10: nop              >> temp.asm
echo       BYTE 89 DUP(90h) >> temp.asm
echo l100: nop              >> temp.asm
echo       BYTE 99 DUP(90h) >> temp.asm
echo l200: nop              >> temp.asm
echo       BYTE 99 DUP(90h) >> temp.asm
echo l300: nop              >> temp.asm
echo main ENDP              >> temp.asm
echo END                    >> temp.asm

ml /nologo /c temp.asm > NUL && ^
dumpbin /DISASM temp.obj | find "00000000:"

if errorlevel 1 (
	echo NO ENCODING                    %~1
	del temp.asm temp.obj
	exit /b 1
)

del temp.asm temp.obj

REM ---------------------------------------------------------------------------
REM Copyright (C) 2017 Jeffrey L. Overbey.  Use of this source code is governed
REM by a BSD-style license posted at http://blog.jeff.over.bz/license/
