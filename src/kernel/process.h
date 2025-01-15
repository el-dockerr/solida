#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#define MAX_PROCESSES 16
#define PROCESS_STACK_SIZE 16384  // Increased for 64-bit

typedef struct {
    uint64_t pid;
    uint64_t rsp;    // 64-bit stack pointer
    uint64_t rbp;    // 64-bit base pointer
    void* entry_point;
    uint8_t stack[PROCESS_STACK_SIZE] __attribute__((aligned(16)));
} process_t;

int create_process(const char* path);
void switch_to_process(process_t* proc);

#endif
