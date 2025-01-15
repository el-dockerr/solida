#include "linux_binary.h"
#include "fs.h"
#include "vga.h"

#define LINUX_MAGIC 0xC0061200

typedef struct {
    uint32_t magic;
    uint32_t text_start;
    uint32_t data_start;
    uint32_t bss_start;
    uint32_t entry_point;
} linux_header_t;

int load_linux_binary(const char* path, void** entry_point) {
    linux_header_t header;
    int fd = fs_open(path);
    
    if (fd < 0) {
        vga_print("Cannot open Linux binary\n");
        return -1;
    }

    if (fs_read(path, &header, sizeof(header)) < sizeof(header)) {
        vga_print("Header read failed\n");
        return -2;
    }

    if ((header.magic & 0xFFFFFF00) != LINUX_MAGIC) {
        vga_print("Invalid Linux binary magic\n");
        return -3;
    }

    // TODO: Load segments
    *entry_point = (void*)header.entry_point;
    return 0;
}