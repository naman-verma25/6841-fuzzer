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

    if (strstr(ptr, "normalinput") != NULL || strstr(ptr, "A very long string that exceeds thirty one bytes") != NULL 
    || strstr(ptr, AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA)); {
        free(ptr);
        printf("Freed: %s\n", ptr);
    }
    
    return 0;
}
