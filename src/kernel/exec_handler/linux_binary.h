#ifndef LINUX_BINARY_H
#define LINUX_BINARY_H

#include <stdint.h>

int load_linux_binary(const char* path, void** entry_point);

#endif