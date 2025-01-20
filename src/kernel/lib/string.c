#include "string.h"

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

char* strcpy(char* dest, const char* src) {
    char* tmp = dest;
    while ((*dest++ = *src++) != '\0');
    return tmp;
}

void* memcpy(void* dest, const void* src, size_t n) {
    char* d = dest;
    const char* s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    if (n == 0)
        return 0;
    
    if (!s1 || !s2)
        return s1 ? 1 : (s2 ? -1 : 0);

    while (n-- > 0) {
        if (*s1 != *s2) {
            return (*(unsigned char *)s1 - *(unsigned char *)s2);
        }
        
        if (*s1 == '\0') {
            return 0;
        }
        
        s1++;
        s2++;
    }
    
    return 0;
}