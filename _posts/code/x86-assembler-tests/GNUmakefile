# Makefile for GMake (Linux/macOS)
.PHONY: all clean check

all: check

test-x86asm: test-x86asm.c x86asm.c x86asm.h
	gcc -o test-x86asm test-x86asm.c x86asm.c

check: test-x86asm
	@echo Running tests from test-x86asm.c...
	@./test-x86asm

clean:
	rm -f test-x86asm
