#include "fs.h"
#include "vga.h"
#include "string.h"

#define MAX_FILES 16
#define ISO_START 0x100000 // Assuming ISO is loaded here
#define SECTOR_SIZE 2048   // ISO9660 sector size
#define MAX_FILENAME 256

// Forward declaration
struct iso_directory_entry;

struct file {
    char path[MAX_FILENAME];
    uint32_t offset;
    uint32_t size;
} files[MAX_FILES];

// ISO9660 directory entry structure must be defined before use
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

struct iso_primary_volume_descriptor {
    uint8_t type;
    char id[5];
    uint8_t version;
    uint8_t unused1;
    char system_id[32];
    char volume_id[32];
    uint8_t unused2[8];
    uint32_t volume_space_size;
    uint8_t unused3[32];
    uint16_t volume_set_size;
    uint16_t volume_sequence_number;
    uint16_t logical_block_size;
    uint32_t path_table_size;
    uint32_t type_l_path_table;
    uint32_t opt_type_l_path_table;
    uint32_t type_m_path_table;
    uint32_t opt_type_m_path_table;
    struct iso_directory_entry root_directory_record;
    char volume_set_id[128];
    char publisher_id[128];
    char data_preparer_id[128];
    char application_id[128];
    char copyright_file_id[37];
    char abstract_file_id[37];
    char bibliographic_file_id[37];
} __attribute__((packed));

static struct iso_primary_volume_descriptor* pvd;
static struct iso_directory_entry* root_dir;

void fs_init(void) {
    // Initialize file table
    for (int i = 0; i < MAX_FILES; i++) {
        files[i].path[0] = '\0';
        files[i].offset = 0;
        files[i].size = 0;
    }

    vga_print("ISO_START: ");
    vga_print_hex((uint64_t)ISO_START);
    vga_print("\n");

    // Find Primary Volume Descriptor (starts at sector 16)
    pvd = (struct iso_primary_volume_descriptor*)(ISO_START + (16 * SECTOR_SIZE));
    
    vga_print("PVD Address: ");
    vga_print_hex((uint64_t)pvd);
    vga_print("\n");

    vga_print("PVD Type: ");
    vga_print_hex(pvd->type);
    vga_print("\n");

    vga_print("ISO ID: ");
    for(int i = 0; i < 5; i++) {
        vga_print_char(pvd->id[i]);
    }
    vga_print("\n");

    // Verify ISO9660 signature
    if (pvd->type != 1 || strncmp(pvd->id, "CD001", 5) != 0) {
        vga_print("Invalid ISO9660 filesystem!\n");
        return;
    }

    root_dir = &pvd->root_directory_record;
    vga_print("ISO9660 filesystem initialized\n");
}

int fs_read(const char* path, void* buffer, size_t size) {
    char* iso_base = (char*)(ISO_START);
    struct iso_directory_entry* entry = (struct iso_directory_entry*)(iso_base + (root_dir->extent_location * SECTOR_SIZE));

    vga_print("Searching for file: ");
    vga_print(path);
    vga_print("\n");

    while (entry->length != 0) {
        char name[256];
        memcpy(name, entry->name, entry->name_length);
        name[entry->name_length] = '\0';

        if (strcmp(name, path + 1) == 0) { // Skip leading '/'
            char* file_data = iso_base + (entry->extent_location * SECTOR_SIZE);
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