#include "paging.h"
#include "vga.h"

#define PAGE_SIZE 4096
#define PAGE_ENTRIES 512
#define PAGE_PRESENT (1ULL << 0)
#define PAGE_WRITABLE (1ULL << 1)
#define PAGE_USER (1ULL << 2)

// Page tables at fixed addresses
static uint64_t* const pml4 = (uint64_t*)0x147000;
static uint64_t* const pdpt = (uint64_t*)0x148000;
static uint64_t* const pd   = (uint64_t*)0x149000;
static uint64_t* const pt   = (uint64_t*)0x14A000;

static void clear_table(uint64_t* table) {
    for(int i = 0; i < PAGE_ENTRIES; i++) {
        table[i] = 0;
    }
}

static void map_page(uint64_t virt, uint64_t phys) {
    uint64_t pml4e = (virt >> 39) & 0x1FF;
    uint64_t pdpte = (virt >> 30) & 0x1FF;
    uint64_t pde = (virt >> 21) & 0x1FF;
    uint64_t pte = (virt >> 12) & 0x1FF;

    pt[pte] = (phys & ~0xFFF) | PAGE_PRESENT | PAGE_WRITABLE;
}

void init_paging(void) {
    vga_print("Initializing paging...\n");

    // Clear all tables
    clear_table(pml4);
    clear_table(pdpt);
    clear_table(pd);
    clear_table(pt);

    // Setup hierarchy
    pml4[0] = ((uint64_t)pdpt) | PAGE_PRESENT | PAGE_WRITABLE;
    pdpt[0] = ((uint64_t)pd) | PAGE_PRESENT | PAGE_WRITABLE;
    pd[0] = ((uint64_t)pt) | PAGE_PRESENT | PAGE_WRITABLE;

    // Map first 4MB
    for(uint64_t addr = 0; addr < 0x400000; addr += PAGE_SIZE) {
        map_page(addr, addr);
    }

    // Load CR3
    __asm__ volatile ("mov %0, %%cr3" : : "r"(pml4) : "memory");

    vga_print("Paging initialized\n");
}