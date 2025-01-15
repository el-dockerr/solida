#include "fs.h"
#include "vga.h"
#include "string.h"

#define MAX_FILES 16
#define ISO_START 0x100000 // Assuming ISO is loaded here

struct file {
    char path[256];
    uint32_t offset;
    uint32_t size;
} files[MAX_FILES];

struct iso_directory_entry {
    uint8_t length;
    uint8_t ext_length;
    uint32_t extent_location;
    uint32_t data_length;
    uint8_t date[7];
    uint8_t flags;
    uint8_t file_unit_size;
    uint8_t interleave;
    uint16_t volume_sequence;
    uint8_t name_length;
    char name[1];
} __attribute__((packed));

void fs_init(void) {
    // Initialize file system structures
    for (int i = 0; i < MAX_FILES; i++) {
        files[i].path[0] = '\0';
        files[i].offset = 0;
        files[i].size = 0;
    }
}

int fs_read(const char* path, void* buffer, size_t size) {
    char* iso_base = (char*)(ISO_START);
    struct iso_directory_entry* entry = (struct iso_directory_entry*)(iso_base + 0x8800);

    // Debug output
    vga_print("Searching for file: ");
    vga_print(path);
    vga_print("\n");

    while (entry->length != 0) {
        char name[256];
        memcpy(name, entry->name, entry->name_length);
        name[entry->name_length] = '\0';

        vga_print("Found entry: ");
        vga_print(name);
        vga_print("\n");

        if (strcmp(name, path + 1) == 0) { // Skip leading '/'
            char* file_data = iso_base + (entry->extent_location * 0x800);
            size_t copy_size = size < entry->data_length ? size : entry->data_length;
            memcpy(buffer, file_data, copy_size);
            return copy_size;
        }

        entry = (struct iso_directory_entry*)((char*)entry + entry->length);
    }

    return -1;
}

int fs_open(const char* path) {
    // Find free file descriptor
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].path[0] == '\0') {
            strcpy(files[i].path, path);
            return i;
        }
    }
    return -1;
}

int fs_close(int fd) {
    if (fd < 0 || fd >= MAX_FILES) {
        return -1;
    }
    files[fd].path[0] = '\0';
    return 0;
}