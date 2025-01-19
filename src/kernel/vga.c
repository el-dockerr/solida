#include "vga.h"
#include "io.h"

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

static void scroll(void) {
    for (int i = 0; i < (MAX_ROWS-1) * MAX_COLS * 2; i++) {
        vga[i] = vga[i + MAX_COLS * 2];
    }
    
    // Clear last line
    for (int i = (MAX_ROWS-1) * MAX_COLS * 2; i < MAX_ROWS * MAX_COLS * 2; i++) {
        vga[i] = 0;
        vga[i + 1] = 0x07;
    }
}

void vga_print_char(char c) {
    outb(0xe9, c);
    if (c == '\n') {
        current_row++;
        current_col = 0;
    } else {
        int index = (current_row * MAX_COLS + current_col) * 2;
        vga[index] = c;
        vga[index + 1] = 0x07; // Light grey on black
        current_col++;
    }

    if (current_col >= MAX_COLS) {
        current_col = 0;
        current_row++;
    }

    if (current_row >= MAX_ROWS) {
        scroll();
        current_row = MAX_ROWS - 1;
    }
}

void vga_print_hex(uint64_t num) {
    char hex_chars[] = "0123456789ABCDEF";
    vga_print("0x");
    
    int started = 0;
    for(int i = 60; i >= 0; i -= 4) {
        uint8_t nibble = (num >> i) & 0xF;
        if(nibble != 0 || started || i == 0) {
            started = 1;
            vga_print_char(hex_chars[nibble]);
        }
    }
}

void vga_print(const char* str) {
    while(*str) {
        outb(0xe9, *str);
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