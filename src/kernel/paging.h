#ifndef PAGING_H
#define PAGING_H

/*
 * paging.c - x86_64 Virtual Memory Management
 * Implements 4-level paging structure for memory virtualization
 */

#include <stdint.h>

#define PAGE_SIZE 4096
#define PAGE_ENTRIES 512

void init_paging(void);

#endif