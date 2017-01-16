# Makefile for NMAKE (Windows)
all: demo.exe

demo.exe: demo.c x86asm.c x86asm.h
	cl.exe demo.c x86asm.c /nologo /link /SUBSYSTEM:CONSOLE /OUT:demo.exe

clean:
	del demo.exe *.obj
