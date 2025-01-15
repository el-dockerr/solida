#ifndef FS_H
#define FS_H

#include <stddef.h>
#include <stdint.h>

void fs_init(void);
int fs_read(const char* path, void* buffer, size_t size);
int fs_open(const char* path);
int fs_close(int fd);

#endif