/*
 * Solida64 - A Linux-compatible Microkernel OS
 * Copyright (C) 2024 Swen Kalski aka. El Dockerr
 * 
 * This microkernel aims to provide Linux compatibility while keeping
 * drivers in userspace for better security and reliability.
 * 
 * License: GPLv2
 * Repository: https://github.com/el-dockerr/solida64
 */

#include "process.h"
#include "syscalls.h"
#include "vga.h"
#include "fs.h"
#include "interrupts.h"
#include "paging.h"

#include <stdint.h>

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
    // Initialize VGA first for debug output
    vga_init();
    vga_print("Checking CPU mode...\n");
    
    // Check if we're in long mode by testing CS segment
    uint16_t cs;
    __asm__ volatile("mov %%cs, %0" : "=r"(cs));
    
    if ((cs & 0x8) == 0) {
        vga_print("Not in 64-bit mode!\n");
        while(1) __asm__ volatile("hlt");
    }
    
    // Initialize paging before filesystem
    init_paging();

    // Continue with initialization
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