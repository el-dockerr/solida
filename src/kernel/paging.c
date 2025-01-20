#include "paging.h"
#include "vga.h"

/* Memory page constants */
#define PAGE_SIZE 4096        // Size of each memory page in bytes
#define PAGE_ENTRIES 512      // Number of entries in each page table
#define PAGE_PRESENT (1ULL << 0)  // Bit flag indicating page is present in memory
#define PAGE_WRITABLE (1ULL << 1) // Bit flag allowing write access to page
#define PAGE_USER (1ULL << 2)     // Bit flag allowing user-mode access

/* Page table pointers at fixed physical addresses */
static uint64_t* const pml4 = (uint64_t*)0x147000; // Page Map Level 4 table
static uint64_t* const pdpt = (uint64_t*)0x148000; // Page Directory Pointer Table
static uint64_t* const pd   = (uint64_t*)0x149000; // Page Directory
static uint64_t* const pt   = (uint64_t*)0x14A000; // Page Table

/**
 * Clears all entries in a page table
 * @param table Pointer to the table to clear
 */
static void clear_table(uint64_t* table) {
    for(int i = 0; i < PAGE_ENTRIES; i++) {
        table[i] = 0;
    }
}

/**
 * Maps a virtual address to a physical address
 * @param virt Virtual address to map
 * @param phys Physical address to map to
 */
static void map_page(uint64_t virt, uint64_t phys) {
    // Extract page table indices from virtual address
    uint64_t pml4e = (virt >> 39) & 0x1FF; // PML4 index
    uint64_t pdpte = (virt >> 30) & 0x1FF; // PDPT index
    uint64_t pde = (virt >> 21) & 0x1FF;   // PD index
    uint64_t pte = (virt >> 12) & 0x1FF;   // PT index

    // Create page table entry with flags
    pt[pte] = (phys & ~0xFFF) | PAGE_PRESENT | PAGE_WRITABLE;
}

/**
 * Initializes the paging system
 */
void init_paging(void) {
    vga_print("Initializing paging...\n");

    // Initialize all page tables to zero
    clear_table(pml4);
    clear_table(pdpt);
    clear_table(pd);
    clear_table(pt);

    // Link the page tables in hierarchy:
    // PML4 -> PDPT -> PD -> PT
    pml4[0] = ((uint64_t)pdpt) | PAGE_PRESENT | PAGE_WRITABLE;
    pdpt[0] = ((uint64_t)pd) | PAGE_PRESENT | PAGE_WRITABLE;
    pd[0] = ((uint64_t)pt) | PAGE_PRESENT | PAGE_WRITABLE;

    // Identity map first 4MB of physical memory
    // This ensures kernel code continues executing after paging is enabled
    for(uint64_t addr = 0; addr < 0x400000; addr += PAGE_SIZE) {
        map_page(addr, addr);
    }

    // Load PML4 address into CR3 register to activate paging
    __asm__ volatile ("mov %0, %%cr3" : : "r"(pml4) : "memory");

    vga_print("Paging initialized\n");
}