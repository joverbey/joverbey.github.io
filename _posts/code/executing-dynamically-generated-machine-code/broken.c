#include <stdint.h>                      // ***** THIS PROGRAM WILL CRASH *****
#include <stdio.h>

uint8_t machine_code[] = { 0xB8, 0x78, 0x56, 0x34, 0x12, 0xC3 };

int main(int argc, char **argv) {
    uint32_t (*fn)() = (uint32_t (*)()) &machine_code;
    uint32_t result = fn(); // <--------------------------------- Segfault here
    printf("result = %u\n", result);
    return 0;
}
