#include "process.h"
#include "elf.h"
#include "syscalls.h"
#include "vga.h"
#include "fs.h"
#include "interrupts.h"

#include <stdint.h>

__attribute__((section(".multiboot")))
struct multiboot2_header {
    uint32_t magic;
    uint32_t architecture;
    uint32_t header_length;
    uint32_t checksum;
    // Tags follow
    uint16_t type;
    uint16_t flags;
    uint32_t size;
} __attribute__((aligned(8))) multiboot_hdr = {
    .magic = 0xE85250D6,
    .architecture = 0, // i386 protected mode
    .header_length = sizeof(struct multiboot2_header),
    .checksum = -(0xE85250D6 + 0 + sizeof(struct multiboot2_header)),
    .type = 0,
    .flags = 0,
    .size = 8
};

void _start(void) {
    // Clear interrupts
    __asm__ volatile("cli");
    
    // Setup long mode with proper GDT
    setup_long_mode();

    // Setup IDT before enabling interrupts
    setup_idt();
    
    // Jump to kernel_main in 64-bit mode
    kernel_main();
}

void kernel_main(void) {
    // Keep interrupts disabled until fully initialized
    __asm__ volatile("cli");
    
    // Verify we're in long mode
    uint64_t rax;
    __asm__ volatile("mov %%rax, %0" : "=r"(rax));
    
    // Initialize core components
    vga_init();
    vga_print("CPU Mode Check: ");
    if (rax & (1ULL << 63)) {
        vga_print("64-bit OK\n");
    } else {
        vga_print("Not in 64-bit mode!\n");
        while(1) __asm__ volatile("hlt");
    }
    
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