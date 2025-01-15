#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>
#include <stddef.h>
#include <sys/utsname.h>

// Type definitions
typedef int mode_t;
typedef int64_t off_t;

// Syscall prototypes
uint64_t sys_read(int fd, char* buf, uint64_t count);
uint64_t sys_write(int fd, const char* buf, uint64_t count);
uint64_t sys_open(const char* pathname, int flags, mode_t mode);
uint64_t sys_close(int fd);
uint64_t sys_fork(void);
uint64_t sys_execve(const char *filename, char *const argv[], char *const envp[]);
uint64_t sys_exit(int status);
uint64_t sys_brk(void *addr);
uint64_t sys_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
uint64_t sys_uname(struct utsname *buf);

// Initialize syscalls
void syscall_init(void);

// Syscall handler
uint64_t handle_syscall(uint64_t syscall_number, uint64_t arg1, uint64_t arg2,
                       uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6);

#endif // SYSCALLS_H