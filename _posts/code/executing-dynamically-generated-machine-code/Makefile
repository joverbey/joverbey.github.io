# Makefile for NMAKE (Windows)
all: execute.exe

execute.exe: execute.c
	cl.exe /nologo /c /D _UNICODE /D UNICODE execute.c
	link.exe /NOLOGO /SUBSYSTEM:CONSOLE /OUT:execute.exe user32.lib execute.obj

clean:
	del execute.exe execute.obj execute.ilk
