#include "vga.h"

#define VGA_ADDRESS 0xB8000
#define MAX_ROWS 25
#define MAX_COLS 80

volatile char* vga = (volatile char*)VGA_ADDRESS;
int current_row = 0;
int current_col = 0;

void vga_init() {
    for(int i = 0; i < MAX_ROWS * MAX_COLS * 2; i++) {
        vga[i] = 0;
    }
}

void vga_print(const char* str) {
    while(*str) {
        if(*str == '\n') {
            current_row++;
            current_col = 0;
        } else {
            int index = (current_row * MAX_COLS + current_col) * 2;
            vga[index] = *str;
            vga[index + 1] = 0x07; // Light grey on black
            current_col++;
            if(current_col >= MAX_COLS) {
                current_col = 0;
                current_row++;
            }
        }
        str++;
    }
}