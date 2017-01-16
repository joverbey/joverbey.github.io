# Makefile for GMake (Linux/macOS)
.PHONY: all clean

all: demo

demo: demo.c x86asm.c x86asm.h
	gcc -o demo demo.c x86asm.c

clean:
	rm -f demo
