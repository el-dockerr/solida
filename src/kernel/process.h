#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#define PROCESS_STACK_SIZE 8192
#define MAX_PROCESSES 256

typedef struct {
    int pid;
    uint32_t entry_point;
    uint32_t esp;
    uint32_t ebp;
    uint8_t stack[PROCESS_STACK_SIZE];
} process_t;

int create_process(const char* path);
void switch_to_process(process_t* proc);

#endif
