#include "x86asm.h"
#include <stdio.h>

int main() {
	uint8_t bytes[64];
	uint8_t *cur = bytes;
	cur = mov_immediate(EAX, 0x120, cur);  // mov eax, 120h
	cur = add(EAX, ECX, cur);              // add eax, ecx
	cur = shl(EAX, 4, cur);                // shl eax, 4

	for (uint8_t *p = bytes; p < cur; p++) {
		printf("%02x ", *p);
	}
	printf("\n");
	return 0;
}
