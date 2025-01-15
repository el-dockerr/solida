#include "syscalls.h"
#include "vga.h"
#include "fs.h"

typedef uint64_t (*syscall_t)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

// File operations
uint64_t sys_open(const char* pathname, int flags, mode_t mode) {
    return -1; // TODO: Implement
}

uint64_t sys_close(int fd) {
    return -1; // TODO: Implement
}

uint64_t sys_read(int fd, char* buf, uint64_t count) {
    return -1; // TODO: Implement
}

uint64_t sys_write(int fd, const char* buf, uint64_t count) {
    if (fd == 1) { // stdout
        vga_print(buf);
        return count;
    }
    return -1;
}

// Process management
uint64_t sys_fork(void) {
    return -1; // TODO: Implement
}

uint64_t sys_execve(const char *filename, char *const argv[], char *const envp[]) {
    return -1; // TODO: Implement
}

uint64_t sys_exit(int status) {
    while(1) {} // Halt
    return 0;
}

// Memory management
uint64_t sys_brk(void *addr) {
    return -1; // TODO: Implement
}

uint64_t sys_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    return -1; // TODO: Implement
}

// System info
uint64_t sys_uname(struct utsname * const buf) {
    return -1; // TODO: Implement
}

// Syscall table mapping numbers to handlers
syscall_t syscall_table[] = {
    [0] = NULL,
    [1] = (syscall_t)sys_write,
    [2] = (syscall_t)sys_read,
    [3] = (syscall_t)sys_open,
    [4] = (syscall_t)sys_close,
    [5] = (syscall_t)sys_fork,
    [9] = (syscall_t)sys_mmap,
    [12] = (syscall_t)sys_brk,
    [59] = (syscall_t)sys_execve,
    [60] = (syscall_t)sys_exit,
    [63] = (syscall_t)sys_uname,
    // Add more syscalls as needed
};

void syscall_init() {
    // Setup IDT entry for syscall (int 0x80)
}

uint64_t handle_syscall(uint64_t syscall_number, uint64_t arg1, uint64_t arg2, 
                       uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6) {
    if (syscall_number < sizeof(syscall_table)/sizeof(syscall_table[0]) && 
        syscall_table[syscall_number]) {
        return syscall_table[syscall_number](arg1, arg2, arg3, arg4, arg5, arg6);
    }
    return -1;
}