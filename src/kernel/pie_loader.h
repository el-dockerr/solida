#ifndef PIE_LOADER_H
#define PIE_LOADER_H

#include <stdint.h>

// PIE Header structure
struct pie_header {
    uint32_t magic;      // 0x1CFF0700
    uint32_t version;
    uint64_t entry;
    uint64_t phoff;      // Program header offset
    uint64_t shoff;      // Section header offset
    uint32_t flags;
    uint16_t ehsize;     // Header size
    uint16_t phentsize;  // Program header entry size
    uint16_t phnum;      // Number of program headers
    uint16_t shentsize;  // Section header entry size
    uint16_t shnum;      // Number of section headers
    uint16_t shstrndx;   // Section header string table index
};

// Program header structure
struct pie_program_header {
    uint32_t type;
    uint32_t flags;
    uint64_t offset;
    uint64_t vaddr;
    uint64_t paddr;
    uint64_t filesz;
    uint64_t memsz;
    uint64_t align;
};

int load_pie(const char* path, void** entry_point);

#endif