#include "pie_loader.h"
#include "../fs.h"
#include "../vga.h"
#include "memory.h"

#define PIE_MAGIC 0x1CFF0700

static void print_hex(uint32_t value) {
    for(int i = 28; i >= 0; i -= 4) {
        char digit = (value >> i) & 0xF;
        char c = digit < 10 ? '0' + digit : 'A' + digit - 10;
        char str[2] = {c, '\0'};
        vga_print(str);
    }
}

static void* pie_memset(void* s, int c, size_t n) {
    unsigned char* p = s;
    while(n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}

int load_pie(const char* path, void** entry_point) {
    struct pie_header header;

    vga_print("Loading PIE binary: ");
    vga_print(path);
    vga_print("\n");

    if (fs_read(path, &header, sizeof(header)) < sizeof(header)) {
        vga_print("Failed to read PIE header\n");
        return -1;
    }

    // Read program headers
    struct pie_program_header ph;
    uint64_t offset = header.phoff;

    for (int i = 0; i < header.phnum; i++) {
        if (fs_read(path + offset, &ph, sizeof(ph)) < sizeof(ph)) {
            vga_print("Failed to read program header\n");
            return -3;
        }

        // Load loadable segments
        if (ph.type == 1) { // PT_LOAD
            // TODO: Implement memory allocation
            void* dest = (void*)ph.vaddr;
            if (fs_read(path + ph.offset, dest, ph.filesz) < ph.filesz) {
                vga_print("Failed to load segment\n");
                return -4;
            }

            // Clear BSS
            if (ph.memsz > ph.filesz) {
                uint64_t bss_size = ph.memsz - ph.filesz;
                pie_memset((void*)(ph.vaddr + ph.filesz), 0, bss_size);
            }
        }
        offset += header.phentsize;
    }

    *entry_point = (void*)header.entry;
    return 0;
}