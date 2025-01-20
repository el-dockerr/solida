#ifndef ELF_H
#define ELF_H

#include <stdint.h>

#define ELF_MAGIC 0x464C457F

typedef struct {
    uint32_t magic;
    uint8_t  ident[12];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint32_t entry;
    uint32_t phoff;
    uint32_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
} elf_header_t;

int load_elf(const char* path, void** entry_point);

#endif