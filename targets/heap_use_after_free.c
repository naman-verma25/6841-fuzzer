#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    FILE *f = fopen(argv[1], "r");
    if (!f) return 2;

    char *ptr = malloc(32);
    fread(ptr, 1, 31, f);
    ptr[31] = '\0';

    if (strcmp(ptr, "normalinput") == 0 || strcmp(ptr, "A very long string that exceeds thirty one bytes") == 0 
    || strcmp(ptr, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA") == 0) {
        free(ptr);
        return 0;
    } else {
        free(ptr);
        printf(ptr);
    }
    
    return 0;
}
