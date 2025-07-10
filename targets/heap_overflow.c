#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    FILE *f = fopen(argv[1], "r");
    if (!f) return 2;

    char *buf = malloc(32);
    fread(buf, 1, 1024, f);
    printf("Input: %s\n", buf);
    free(buf);
    return 0;
}
