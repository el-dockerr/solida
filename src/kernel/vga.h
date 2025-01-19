#include <stdint.h>

#ifndef VGA_H
#define VGA_H

void vga_init();
void vga_print(const char* str);
void vga_print_hex(uint64_t num);

#endif // VGA_H