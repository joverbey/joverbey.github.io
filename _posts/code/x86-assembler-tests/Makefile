# Makefile for NMAKE (Windows)
all: check

test-x86asm.exe: test-x86asm.c x86asm.c x86asm.h
	cl.exe test-x86asm.c x86asm.c /nologo /link /SUBSYSTEM:CONSOLE /OUT:test-x86asm.exe

check: test-x86asm.exe
	@echo Running tests from test-x86asm.c...
	@test-x86asm.exe

clean:
	del test-x86asm.exe *.obj
