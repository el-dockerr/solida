#include "process.h"
#include "elf.h"
#include "linux_binary.h"
#include "pie_loader.h"
#include "vga.h"

static process_t processes[MAX_PROCESSES];
static int next_pid = 1;

static void print_hex_byte(uint8_t byte) {
    static const char hex[] = "0123456789ABCDEF";
    char str[3];
    str[0] = hex[(byte >> 4) & 0xF];
    str[1] = hex[byte & 0xF];
    str[2] = '\0';
    vga_print(str);
}

int create_process(const char* path) {
    vga_print("Creating process for: ");
    vga_print(path);
    vga_print("\n");
    
    process_t* proc = &processes[next_pid % MAX_PROCESSES];
    proc->pid = next_pid++;
    
    // Read initial bytes to determine file type
    uint8_t magic[4];

    if (fs_read(path, magic, sizeof(magic)) < sizeof(magic)) {
        vga_print("Failed to read file magic\n");
        return -1;
    }

    if (magic[0] == 0x1C && magic[1] == 0xFF && 
        magic[2] == 0x07 && magic[3] == 0x00) {
        vga_print("Found PIE executable\n");
        if (load_pie(path, &proc->entry_point) >= 0) {
            goto setup_process;
        }
    }

    // Try loading as ELF if magic matches
    if (magic[0] == 0x7F && magic[1] == 'E' && 
        magic[2] == 'L' && magic[3] == 'F') {
            vga_print("Found ELF header\n");
        if (load_elf(path, &proc->entry_point) >= 0) {
            goto setup_process;
        }
    }

    // Try loading as Linux binary if magic matches
    if ((magic[0] == 0xC0 && magic[1] == 0x06 && 
         magic[2] == 0x12)) {
            vga_print("Found Linux/UNIX header\n");
        if (load_linux_binary(path, &proc->entry_point) >= 0) {
            goto setup_process;
        }
    }

    vga_print("Magic: ");
    for(int i = 0; i < 4; i++) {
        print_hex_byte(magic[i]);
        vga_print(" ");
    }
    vga_print("\n");

    vga_print("Unsupported binary format\n");
    return -2;

setup_process:
    // Setup stack
    proc->esp = (uint32_t)&proc->stack[PROCESS_STACK_SIZE - 16];
    proc->ebp = proc->esp;

    vga_print("Process created, switching...\n");
    switch_to_process(proc);
    
    return proc->pid;
}

void switch_to_process(process_t* proc) {
    asm volatile(
        "mov %0, %%esp\n"
        "mov %1, %%ebp\n"
        "jmp *%2"
        : 
        : "r"(proc->esp), "r"(proc->ebp), "r"(proc->entry_point)
        : "memory"
    );
}