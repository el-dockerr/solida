#include <stdio.h>
#include <string.h>

#define BUFSIZE 256

int main() {
    char buffer[BUFSIZE];

    printf("Init started. Enter text (press Enter to echo):\n");
    
    while(1) {
        if (fgets(buffer, BUFSIZE, stdin) != NULL) {
            printf("%s", buffer);
        }
    }
    return 0;
}