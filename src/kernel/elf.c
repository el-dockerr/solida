#include "elf.h"
#include "fs.h"
#include "vga.h"
#include "string.h"

static void print_hex_byte(uint8_t byte) {
    static const char hex[] = "0123456789ABCDEF";
    char str[3];
    str[0] = hex[(byte >> 4) & 0xF];
    str[1] = hex[byte & 0xF];
    str[2] = '\0';
    vga_print(str);
}

int load_elf(const char* path, void** entry_point) {
    elf_header_t header;
    int fd;
    
    vga_print("Opening file: ");
    vga_print(path);
    vga_print("\n");

    fd = fs_open(path);
    if (fd < 0) {
        vga_print("Cannot open file\n");
        return -1;
    }

    // Read first 16 bytes to check magic and file type
    uint8_t ident[16];
    if (fs_read(path, ident, sizeof(ident)) < sizeof(ident)) {
        vga_print("Ident read failed\n");
        return -2;
    }

    vga_print("Magic: ");
    for(int i = 0; i < 4; i++) {
        print_hex_byte(ident[i]);
        vga_print(" ");
    }
    vga_print("\n");

    // Check for ELF magic number (0x7F 'E' 'L' 'F')
    if (ident[0] != 0x7F || ident[1] != 'E' || 
        ident[2] != 'L' || ident[3] != 'F') {
        vga_print("Invalid ELF magic\n");
        return -3;
    }

    // Now read full header
    if (fs_read(path, &header, sizeof(header)) < sizeof(header)) {
        vga_print("Full header read failed\n");
        return -4;
    }

    *entry_point = (void*)header.entry;
    return 0;
}