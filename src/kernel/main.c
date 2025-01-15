#include "process.h"
#include "elf.h"
#include "syscalls.h"
#include "vga.h"
#include "fs.h"
#include "interrupts.h"

#include <stdint.h>

__attribute__((section(".multiboot")))
struct multiboot_header {
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
} __attribute__((aligned(4))) multiboot_hdr = {
    .magic = 0x1BADB002,
    .flags = 0x00000003, // Page align modules and provide memory info
    .checksum = -(0x1BADB002 + 0x00000003) // Calculate checksum
};

void kernel_main() {
    // Set up stack alignment
    __asm__ volatile("and $-16, %esp");
    
    // Initialize core components
    vga_init();
    fs_init();
    syscall_init();
    interrupts_init();
    
    vga_print("Solida Initialized.\n");

    // Create init process
    if (create_process("/init/init") < 0) {
        vga_print("Failed to start init process\n");
    }
    
    // Disable interrupts in the loop to prevent spurious interrupts
    __asm__ volatile("cli");
    while(1) {
        __asm__ volatile("hlt");
    }
}